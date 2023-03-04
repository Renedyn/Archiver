#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <bitset>

#include "buffer_read.h"
#include "bit_writter.h"
#include "encoder.h"

const int16_t FILENAME_END = 256;
const int16_t ONE_MORE_FILE = 257;
const int16_t ARCHIVE_END = 258;

class IncorrectRequest : public std::exception {};
class IncorrectFlag : public std::exception {};
class InvalidArchiv : public std::exception {};

class ArgsParser {
public:
    ArgsParser(int argc, char** argv) {
        if (argc <= 1) {
            throw IncorrectRequest();
        }
        file_name_ = std::string(argv[0]);

        if (argc == 2 && std::string(argv[1]) == "-h") {
            return;
        }
        command_ = std::string(argv[1]);
        archive_name_ = std::string(argv[2]);
        if (command_ != "-c" && command_ != "-d" && command_ != "-h") {
            throw IncorrectFlag();
        }
        if (command_ == "-h" && argc != 2) {
            throw IncorrectRequest();
        }
        for (size_t i = 3; i < static_cast<size_t>(argc); ++i) {
            if (argv[i][0] == '-') {
                throw IncorrectRequest();
            }
            files_.push_back(argv[i]);
        }
    }

    const std::string& Command() const {
        return command_;
    }

    const std::string& Help() const {
        return help_message_;
    }

    const std::string& ArchiveName() const {
        return archive_name_;
    }

    const std::string& FileName() const {
        return file_name_;
    }

    const std::vector<std::string>& Files() const {
        return files_;
    }

private:
    const std::string help_message_ =
        "Это архиватор ABOBA_ARC. Пользоваться строго под наблюдением специалистов";  // TODO
    std::string command_;
    std::string archive_name_;
    std::string file_name_;
    std::vector<std::string> files_;
};

int main(int argc, char** argv) {
    try {
        ArgsParser parser(argc, argv);
        if (parser.Command() == "-c") {
            std::ofstream file_writter(parser.ArchiveName(), std::ios::binary);
            BufferWritter writer(file_writter);

            for (size_t file = 0; file < parser.Files().size(); ++file) {
                std::unordered_map<int16_t, size_t> count_of_chars;
                count_of_chars[FILENAME_END] = 1;
                count_of_chars[ONE_MORE_FILE] = 1;
                count_of_chars[ARCHIVE_END] = 1;

                for (char i : parser.Files()[file]) {
                    ++count_of_chars[i];
                }

                std::ifstream reader(parser.Files()[file], std::ios::binary);
                BufferRead reader_count(reader);
                for (auto i = reader_count.ReadChar(); !reader_count.IsFileEnded(); i = reader_count.ReadChar()) {
                    ++count_of_chars[i];
                }
                reader.close();

                auto res = CanonicalCode(Haffman(count_of_chars));
                std::unordered_map<int16_t, std::pair<uint64_t, size_t>> codes;
                for (size_t i = 0; i < res.Size(); ++i) {
                    codes[res.chars[i]] = {res.codes[i], res.size[i]};
                }
                writer.WriteSBits(res.Size());

                for (int16_t i : res.chars) {
                    writer.WriteSBits(i);
                }
                std::vector<size_t> count_of_lens(res.size.back());
                for (size_t len : res.size) {
                    ++count_of_lens[len - 1];
                }
                for (size_t cnt : count_of_lens) {
                    writer.WriteSBits(cnt);
                }
                for (char i : parser.Files()[file]) {
                    auto [code, len] = codes[static_cast<int16_t>(i)];
                    writer.WriteKBits(code, len);
                }
                {
                    auto [code, len] = codes[static_cast<int16_t>(FILENAME_END)];
                    writer.WriteKBits(code, len);
                }
                reader.open(parser.Files()[file], std::ios::binary);
                BufferRead reader_chars(reader);
                for (int16_t i = reader_chars.ReadChar(); !reader_chars.IsFileEnded(); i = reader_chars.ReadChar()) {
                    auto [code, len] = codes[i];
                    writer.WriteKBits(code, len);
                }
                if (file + 1 < parser.Files().size()) {
                    auto [code, len] = codes[static_cast<int16_t>(ONE_MORE_FILE)];
                    writer.WriteKBits(code, len);
                } else {
                    auto [code, len] = codes[static_cast<int16_t>(ARCHIVE_END)];
                    writer.WriteKBits(code, len);
                }
                reader.close();
            }

            writer.ClearBuffer();
            file_writter.close();
        } else if (parser.Command() == "-d") {
            std::ifstream file_reader(parser.ArchiveName(), std::ios::binary);
            BufferRead reader(file_reader);

            for (size_t file = 0;; ++file) {
                std::unordered_map<int16_t, size_t> lens;
                size_t chars_cnt = reader.ReadSBits();
                CanonCode canon_code;
                canon_code.chars.resize(chars_cnt);
                canon_code.size.resize(chars_cnt);
                canon_code.codes.resize(chars_cnt);

                for (size_t i = 0; i < chars_cnt; ++i) {
                    canon_code.chars[i] = reader.ReadSBits();
                }
                if (reader.IsFileEnded()) {
                    throw InvalidArchiv();
                }
                size_t sum_cnt = 0;
                for (size_t len = 1;; ++len) {
                    size_t cnt = reader.ReadSBits();
                    if (reader.IsFileEnded()) {
                        throw InvalidArchiv();
                    }
                    for (size_t i = sum_cnt; i < sum_cnt + cnt; ++i) {
                        canon_code.size[i] = len;
                    }
                    sum_cnt += cnt;
                    if (sum_cnt == chars_cnt) {
                        break;
                    }
                }
                Trie trie = Trie();
                for (size_t i = 0; i < chars_cnt; ++i) {
                    if (i == 0) {
                        canon_code.codes[i] = 0;
                    } else {
                        canon_code.codes[i] = (canon_code.codes[i - 1] + 1)
                                              << (canon_code.size[i] - canon_code.size[i - 1]);
                    }
                    trie.Add(canon_code.codes[i], canon_code.chars[i], canon_code.size[i]);
                }
                std::string file_name;
                int16_t symb = 0;
                for (bool bit = reader.ReadRBit(); !reader.IsFileEnded(); bit = reader.ReadRBit()) {
                    if (trie.Go(bit, symb)) {
                        if (symb == FILENAME_END) {
                            break;
                        }
                        file_name += static_cast<char>(symb);
                    }
                }
                std::ofstream file_writter(file_name, std::ios::binary);
                BufferWritter writter(file_writter);

                for (bool bit = reader.ReadRBit(); !reader.IsFileEnded(); bit = reader.ReadRBit()) {
                    if (trie.Go(bit, symb)) {
                        if (symb == ONE_MORE_FILE || symb == ARCHIVE_END) {
                            break;
                        }
                        writter.WriteChar(static_cast<char>(symb));
                    }
                }
                writter.ClearBuffer();
                file_writter.close();
                if (symb == ONE_MORE_FILE) {
                    continue;
                } else if (symb == ARCHIVE_END) {
                    break;
                } else {
                    throw InvalidArchiv();
                }
            }

            file_reader.close();
        } else {
            std::cout << parser.Help() << std::endl;
        }
        return 0;
    } catch (IncorrectFlag& exc) {
        std::cout << "Некорректный флаг" << std::endl;
        return 111;
    } catch (IncorrectRequest& exc) {
        std::cout << "Некорректный запрос" << std::endl;
        return 111;
    } catch (InvalidArchiv& exc) {
        std::cout << "Повреждённый архив" << std::endl;
        return 111;
    }
}
