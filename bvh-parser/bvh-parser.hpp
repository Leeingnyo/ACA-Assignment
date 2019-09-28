#include <string>
#include <deque>
#include <vector>
#include <memory>
#include <sstream>

namespace inyong_bvh {

enum class BvhChannels { X_P, X_R, Y_P, Y_R, Z_P, Z_R };

class BvhHierarchy;
class BvhMotion;

class Bvh {
public:
    std::shared_ptr<BvhHierarchy> hierarchy;
    std::shared_ptr<BvhMotion> motion;
};

class BvhJoint {
public:
    std::string id;
    std::vector<BvhChannels> channels;
    double x_offset;
    double y_offset;
    double z_offset;
    int number_of_channels;
    std::vector<std::shared_ptr<BvhJoint>> child_joints;
};

class BvhEndSite {
public:
    double x_offset;
    double y_offset;
    double z_offset;
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
    CHANNEL_VALUE, // Xposition Yposition Zposition Xrelation Yrelation Zrelation
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
public:
    std::shared_ptr<BvhJoint> root;
};

class BvhMotion {
public:
    int number_of_frames;
    float frame_time;
    std::vector<float> motion_data;
};

class BvhParser {
    std::string value;
    bool is_number;

    void clear_internal_variables() {
        value = std::string();
        is_number = false;
    }

    void consume(std::deque<BvhToken>& tokens, BvhEnumToken token);
    double get_number(std::deque<BvhToken>& tokens);
    int get_number_as_int(std::deque<BvhToken>& tokens);
    BvhChannels get_channel_value(std::deque<BvhToken>& tokens);
    std::string get_id(std::deque<BvhToken>& tokens);

    std::shared_ptr<BvhJoint> joint(std::deque<BvhToken>& tokens);
    std::shared_ptr<BvhHierarchy> hierarchy(std::deque<BvhToken>& tokens);
    std::shared_ptr<BvhMotion> motion(std::deque<BvhToken>& tokens);
public:
    std::shared_ptr<std::deque<BvhToken>> scan_from_string(std::string string);
    std::shared_ptr<Bvh> parse_from_tokens(const std::deque<BvhToken>& tokens);
};

}
