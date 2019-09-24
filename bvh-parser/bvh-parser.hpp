#include <string>
#include <deque>
#include <vector>
#include <memory>
#include <sstream>

namespace inyong_bvh {

enum class BvhRotation { X, Y, Z };
enum class BvhPosition { X, Y, Z };

class Bvh {
};

class BvhJoint {
};

class BvhEndSite {
};

enum class BvhEnumToken {
    HIERARCHY, // HIERARCHY
    ROOT, // ROOT
    OPEN_PARENTHESIS, // {
    CLOSE_PARENTHESIS, // }
    OFFSET, // OFFSET
    CHANNELS, // CHANNELS
    NUMBER, // 3 3.14
    ID, // hello world
    POSITION, // Xposition Yposition Zposition
    ROTATION, // Xrelation Yrelation Zrelation
    JOINT, // JOINT
    END, // End
    SITE, // Site
    MOTION, // MOTION
    FRAMES, // Frames
    COLON, // :
    FRAME, // Frame
    TIME, // Time
};

const char TOKEN_NAMES[][20] = {
    "'HIERARCHY'",
    "'ROOT'",
    "'{'",
    "'}'",
    "'OFFSET'",
    "'CHANNELS'",
    "number",
    "id",
    "'POSITION'",
    "'ROTATION'",
    "'JOINT'",
    "'End'",
    "'Site'",
    "'MOTION'",
    "'Frames'",
    "':'",
    "'Frame'",
    "'Time'",
};

class BvhToken {
public:
    BvhEnumToken token;
    std::string value;
    int line, column;
    BvhToken(int _line, int _column, BvhEnumToken _token) : line(_line), column(_column), token(_token) {
    }
    BvhToken(int _line, int _column, BvhEnumToken _token, std::string _value)
            : line(_line), column(_column), token(_token), value(_value) {
    }
};

class BvhHierarchy {
};

class BvhMotion {
};

class BvhParser {
    std::string value;
    bool is_number;

    void clear_internal_variables() {
        value = std::string();
        is_number = false;
    }

    void consume(std::deque<BvhToken>& tokens, BvhEnumToken token) {
        const auto& consumed = tokens[0];
        if (consumed.token != token) {
            std::stringstream ss;
            ss << std::endl << "Position: " << consumed.line << ", " << consumed.column << std::endl <<
                    "the token type is mismatched." << std::endl <<
                    "expected " << TOKEN_NAMES[static_cast<int>(token)] << " but " << TOKEN_NAMES[static_cast<int>(consumed.token)] << " is appeared.";
            throw std::runtime_error(ss.str());
        }
        tokens.pop_front();
    }
    double get_number(std::deque<BvhToken>& tokens) {
        const auto& peek = tokens[0];
        double d = std::stod(peek.value);
        tokens.pop_front();
        return d;
    }
    int get_number_as_int(std::deque<BvhToken>& tokens) {
        const auto& peek = tokens[0];
        double d = std::stoi(peek.value);
        tokens.pop_front();
        return d;
    }
    std::string get_id(std::deque<BvhToken>& tokens) {
        const auto& peek = tokens[0];
        auto id = tokens[0].value;
        tokens.pop_front();
        return id;
    }

    BvhJoint joint(std::deque<BvhToken>& tokens) {
        bool is_end_site = false;
        std::string id;

        const auto& peek = tokens[0];

        if (peek.token == BvhEnumToken::ROOT) {
            consume(tokens, BvhEnumToken::ROOT);
            id = get_id(tokens);
        }
        else if (peek.token == BvhEnumToken::JOINT) {
            consume(tokens, BvhEnumToken::JOINT);
            id = get_id(tokens);
        }
        else if (peek.token == BvhEnumToken::END) {
            consume(tokens, BvhEnumToken::END);
            consume(tokens, BvhEnumToken::SITE);
            is_end_site = true;
        }

        consume(tokens, BvhEnumToken::OPEN_PARENTHESIS);
        consume(tokens, BvhEnumToken::OFFSET);
        double x_offset = get_number(tokens);
        double y_offset = get_number(tokens);
        double z_offset = get_number(tokens);
        if (!is_end_site) {
            consume(tokens, BvhEnumToken::CHANNELS);
            int number_of_channels = get_number_as_int(tokens);
            for (int i = 0; i < number_of_channels; i++) {
                if (tokens[0].token == BvhEnumToken::POSITION) consume(tokens, BvhEnumToken::POSITION);
                else consume(tokens, BvhEnumToken::ROTATION);
                // TODO 사용해야함
            }
            std::vector<BvhJoint> child_joints;
            while (tokens[0].token == BvhEnumToken::JOINT || tokens[0].token == BvhEnumToken::END) {
                child_joints.push_back(joint(tokens));
            }
        }
        consume(tokens, BvhEnumToken::CLOSE_PARENTHESIS);
    }
    BvhHierarchy hierarchy(std::deque<BvhToken>& tokens) {
        consume(tokens, BvhEnumToken::HIERARCHY);
        auto root = joint(tokens);
    }
    BvhMotion motion(std::deque<BvhToken>& tokens) {
        int number_of_frames;
        double frame_time;

        consume(tokens, BvhEnumToken::MOTION);
        consume(tokens, BvhEnumToken::FRAMES);
        consume(tokens, BvhEnumToken::COLON);
        number_of_frames = get_number_as_int(tokens);
        consume(tokens, BvhEnumToken::FRAME);
        consume(tokens, BvhEnumToken::TIME);
        consume(tokens, BvhEnumToken::COLON);
        frame_time = get_number(tokens);
        while (tokens.size()) {
            consume(tokens, BvhEnumToken::NUMBER);
        }
    }
public:
    std::shared_ptr<std::deque<BvhToken>> scan_from_string(std::string string) {
        auto deque = std::make_shared<std::deque<BvhToken>>();
        int length = string.length();
        clear_internal_variables();
        int line;
        int column;

        for (int pos = 0, line = 0, column = 0; pos < length; pos++, column++) {
            const char& current = string[pos];

            if (current == '\n') {
                line++;
                column = 0;
            }

            if (current == ' ' || current == '\n' || current == '\t' || current == ':' || current == '{' || current == '}') {
                if (value.length()) {
                    if (is_number) {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::NUMBER, value));
                    }
                    else {
                        if (value == "HIERARCHY") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::HIERARCHY));
                        }
                        else if (value == "ROOT") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::ROOT));
                        }
                        else if (value == "OFFSET") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::OFFSET));
                        }
                        else if (value == "CHANNELS") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNELS));
                        }
                        else if (value == "Xposition") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::POSITION, value));
                        }
                        else if (value == "Yposition") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::POSITION, value));
                        }
                        else if (value == "Zposition") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::POSITION, value));
                        }
                        else if (value == "Xrotation") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::ROTATION, value));
                        }
                        else if (value == "Yrotation") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::ROTATION, value));
                        }
                        else if (value == "Zrotation") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::ROTATION, value));
                        }
                        else if (value == "JOINT") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::JOINT));
                        }
                        else if (value == "End") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::END));
                        }
                        else if (value == "Site") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::SITE));
                        }
                        else if (value == "MOTION") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::MOTION));
                        }
                        else if (value == "Frames") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::FRAMES));
                        }
                        else if (value == "Frame") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::FRAME));
                        }
                        else if (value == "Time") {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::TIME));
                        }
                        else {
                            deque->push_back(BvhToken(line, column, BvhEnumToken::ID, value));
                        }
                    }
                    clear_internal_variables();
                }

                if (current == ':') {
                    deque->push_back(BvhToken(line, column, BvhEnumToken::COLON));
                }
                else if (current == '{') {
                    deque->push_back(BvhToken(line, column, BvhEnumToken::OPEN_PARENTHESIS));
                }
                else if (current == '}') {
                    deque->push_back(BvhToken(line, column, BvhEnumToken::CLOSE_PARENTHESIS));
                }

                continue;
            }

            if (current == '-' || current == '.' || (current >= '0' && current <= '9')) {
                value += current;
                is_number = true;
                continue;
            }

            if (current >= 'A' && 'Z' >= current || current >= 'a' || 'z' >= current || current == '_') {
                value += current;
                continue;
            }
        }

        return deque;
    }

    std::shared_ptr<Bvh> parse_from_tokens(const std::deque<BvhToken>& tokens);
};

}
