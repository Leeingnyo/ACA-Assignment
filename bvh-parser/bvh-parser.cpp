#include <sstream>

#include "./bvh-parser.hpp"

using namespace inyong_bvh;

void BvhParser::consume(std::deque<BvhToken>& tokens, BvhEnumToken token) {
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
double BvhParser::get_number(std::deque<BvhToken>& tokens) {
    const auto& peek = tokens[0];
    double d = std::stod(peek.value);
    tokens.pop_front();
    return d;
}
int BvhParser::get_number_as_int(std::deque<BvhToken>& tokens) {
    const auto& peek = tokens[0];
    double d = std::stoi(peek.value);
    tokens.pop_front();
    return d;
}
BvhChannels BvhParser::get_channel_value(std::deque<BvhToken>& tokens) {
    const auto& peek = tokens[0];
    auto channel = peek.value;
    tokens.pop_front();
    if (channel == "Xposition") {
        return BvhChannels::X_P;
    }
    else if (channel == "Yposition") {
        return BvhChannels::Y_P;
    }
    else if (channel == "Zposition") {
        return BvhChannels::Z_P;
    }
    else if (channel == "Xrotation") {
        return BvhChannels::X_R;
    }
    else if (channel == "Yrotation") {
        return BvhChannels::Y_R;
    }
    else if (channel == "Zrotation") {
        return BvhChannels::Z_R;
    }
}
std::string BvhParser::get_id(std::deque<BvhToken>& tokens) {
    const auto& peek = tokens[0];
    auto id = peek.value;
    tokens.pop_front();
    return id;
}

std::shared_ptr<BvhJoint> BvhParser::joint(std::deque<BvhToken>& tokens) {
    bool is_end_site = false;
    std::string id;
    std::shared_ptr<BvhJoint> joint = std::make_shared<BvhJoint>();

    const auto& peek = tokens[0];

    if (peek.token == BvhEnumToken::ROOT) {
        consume(tokens, BvhEnumToken::ROOT);
        joint->id = get_id(tokens);
    }
    else if (peek.token == BvhEnumToken::JOINT) {
        consume(tokens, BvhEnumToken::JOINT);
        joint->id = get_id(tokens);
    }
    else if (peek.token == BvhEnumToken::END) {
        consume(tokens, BvhEnumToken::END);
        consume(tokens, BvhEnumToken::SITE);
        is_end_site = true;
    }

    consume(tokens, BvhEnumToken::OPEN_PARENTHESIS);
    consume(tokens, BvhEnumToken::OFFSET);
    joint->x_offset = get_number(tokens);
    joint->y_offset = get_number(tokens);
    joint->z_offset = get_number(tokens);
    if (!is_end_site) {
        consume(tokens, BvhEnumToken::CHANNELS);
        joint->number_of_channels = get_number_as_int(tokens);
        for (int i = 0; i < joint->number_of_channels; i++) {
            joint->channels.push_back(get_channel_value(tokens));
        }
        while (tokens[0].token == BvhEnumToken::JOINT || tokens[0].token == BvhEnumToken::END) {
            joint->child_joints.push_back(BvhParser::joint(tokens));
        }
    }
    consume(tokens, BvhEnumToken::CLOSE_PARENTHESIS);
    return joint;
}

std::shared_ptr<BvhHierarchy> BvhParser::hierarchy(std::deque<BvhToken>& tokens) {
    std::shared_ptr<BvhHierarchy> hierarchy = std::make_shared<BvhHierarchy>();
    consume(tokens, BvhEnumToken::HIERARCHY);
    hierarchy->root = joint(tokens);
    return hierarchy;
}

std::shared_ptr<BvhMotion> BvhParser::motion(std::deque<BvhToken>& tokens) {
    std::shared_ptr<BvhMotion> motion = std::make_shared<BvhMotion>();

    consume(tokens, BvhEnumToken::MOTION);
    consume(tokens, BvhEnumToken::FRAMES);
    consume(tokens, BvhEnumToken::COLON);
    motion->number_of_frames = get_number_as_int(tokens);
    consume(tokens, BvhEnumToken::FRAME);
    consume(tokens, BvhEnumToken::TIME);
    consume(tokens, BvhEnumToken::COLON);
    motion->frame_time = get_number(tokens);

    while (tokens.size()) {
        motion->motion_data.push_back(get_number(tokens));
    }

    return motion;
}

std::shared_ptr<std::deque<BvhToken>> BvhParser::scan_from_string(std::string string) {
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
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
                    }
                    else if (value == "Yposition") {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
                    }
                    else if (value == "Zposition") {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
                    }
                    else if (value == "Xrotation") {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
                    }
                    else if (value == "Yrotation") {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
                    }
                    else if (value == "Zrotation") {
                        deque->push_back(BvhToken(line, column, BvhEnumToken::CHANNEL_VALUE, value));
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

std::shared_ptr<Bvh> BvhParser::parse_from_tokens(const std::shared_ptr<std::deque<BvhToken>>& tokens) {
    std::deque<BvhToken> tokens_for_consume;
    tokens_for_consume.assign((*tokens).begin(), (*tokens).end());
    std::shared_ptr<Bvh> bvh = std::make_shared<Bvh>();
    bvh->hierarchy = hierarchy(tokens_for_consume);
    bvh->motion = motion(tokens_for_consume);
    return bvh;
}

/*
HIERARCHY
ROOT pelvis
{
	OFFSET 0.000000 0.000000 0.000000
	CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation
	JOINT lfemur
	{
		OFFSET 11.446000 0.000000 0.000000
		CHANNELS 3 Zrotation Xrotation Yrotation
		JOINT ltibia
		{
			OFFSET 0.000000 -42.030400 0.000000
			CHANNELS 3 Zrotation Xrotation Yrotation
			JOINT lfoot
			{
				OFFSET 0.000000 -39.930200 0.000000
				CHANNELS 3 Zrotation Xrotation Yrotation
				JOINT ltoes
				{
					OFFSET 0.000000 0.000000 11.775800
					CHANNELS 3 Zrotation Xrotation Yrotation
					End Site
					{
					    OFFSET 0.000000 0.000000 5.000000
					}
				}
			}
		}
	}
	JOINT rfemur
	{
		OFFSET -11.446000 0.000000 0.000000
		CHANNELS 3 Zrotation Xrotation Yrotation
		JOINT rtibia
		{
			OFFSET 0.000000 -42.030400 0.000000
			CHANNELS 3 Zrotation Xrotation Yrotation
			JOINT rfoot
			{
				OFFSET 0.000000 -39.930200 0.000000
				CHANNELS 3 Zrotation Xrotation Yrotation
				JOINT rtoes
				{
					OFFSET 0.000000 0.000000 11.775800
					CHANNELS 3 Zrotation Xrotation Yrotation
					End Site
					{
					    OFFSET 0.000000 0.000000 5.000000
					}
				}
			}
		}
	}
	JOINT thorax
	{
		OFFSET 0.000000 11.255600 -5.000000
		CHANNELS 3 Zrotation Xrotation Yrotation
		JOINT head
		{
			OFFSET 0.000000 43.799300 0.000000
			CHANNELS 3 Zrotation Xrotation Yrotation
			End Site
			{
			    OFFSET 0.000000 5.000000 0.000000
			}
		}
		JOINT lclavicle
		{
			OFFSET 0.000000 39.863000 0.000000
			CHANNELS 3 Zrotation Xrotation Yrotation
			JOINT lhumerus
			{
				OFFSET 17.380600 4.958300 0.000000
				CHANNELS 3 Zrotation Xrotation Yrotation
				JOINT lradius
				{
					OFFSET 0.000000 -27.561300 0.000000
					CHANNELS 3 Zrotation Xrotation Yrotation
					JOINT lhand
					{
						OFFSET 0.000000 -26.933000 0.000000
						CHANNELS 3 Zrotation Xrotation Yrotation
						End Site
						{
						    OFFSET 0.000000 -5.000000 0.000000
						}
					}
				}
			}
		}
		JOINT rclavicle
		{
			OFFSET 0.000000 39.863000 0.000000
			CHANNELS 3 Zrotation Xrotation Yrotation
			JOINT rhumerus
			{
				OFFSET -17.380600 4.958300 0.000000
				CHANNELS 3 Zrotation Xrotation Yrotation
				JOINT rradius
				{
					OFFSET 0.000000 -27.561300 0.000000
					CHANNELS 3 Zrotation Xrotation Yrotation
					JOINT rhand
					{
						OFFSET 0.000000 -26.933000 0.000000
						CHANNELS 3 Zrotation Xrotation Yrotation
						End Site
						{
						    OFFSET 0.000000 -5.000000 0.000000
						}
					}
				}
			}
		}
	}
}
MOTION
Frames: 3
Frame Time: 0.008342
59.293900 87.796900 4.760200 -0.467276 -1.378122 -89.563821 -5.641307 2.819435 -8.651036 -0.000000 9.240240 -0.000000 0.000000 3.555270 6.177856 0.000000 -1.301336 -0.000000 -1.803037 2.742629 -3.741876 -0.000000 9.274983 -0.000000 0.000000 3.290364 2.906258 0.000000 -29.528157 0.000000 -4.531867 0.132349 0.213401 15.628959 24.623904 -0.608822 0.394562 0.000000 3.487028 83.769402 0.553653 62.001030 -0.000001 -12.781487 -55.037055 5.615823 0.703335 0.000000 0.095169 -0.000000 -0.923419 -78.312883 9.001808 -58.564139 0.000000 -20.003803 36.764996 -3.936911 -2.063559 -0.000000
59.291700 87.793600 4.752400 -0.475906 -1.381233 -89.556452 -5.638376 2.830551 -8.625235 0.000000 9.236131 -0.000000 -0.000000 3.523541 6.156061 0.000000 -1.301682 -0.000000 -1.792531 2.748974 -3.691834 -0.000000 9.276495 0.000000 0.000000 3.262773 2.860635 0.000000 -29.527798 0.000000 -4.547519 0.143945 0.207558 15.646355 24.636287 -0.589535 0.469632 -0.000000 3.531537 83.641881 0.487220 62.085857 -0.000000 -12.806394 -55.155542 5.630933 0.721572 0.000000 0.133438 -0.000000 -0.934718 -78.356085 8.980916 -58.621133 0.000000 -19.969767 36.707936 -3.798525 -2.110204 -0.000000
59.284100 87.792700 4.735700 -0.468226 -1.370968 -89.577780 -5.616747 2.843706 -8.610767 0.000000 9.200918 -0.000000 -0.000000 3.503068 6.153399 -0.000000 -1.302785 0.000000 -1.773209 2.721528 -3.689529 -0.000000 9.319831 -0.000000 -0.000000 3.257897 2.830402 -0.000000 -29.528157 -0.000000 -4.536401 0.106276 0.231925 15.644609 24.663646 -0.606920 0.409234 0.000000 3.553416 83.667533 0.489506 61.762180 0.000000 -12.898223 -54.860654 5.586698 0.818060 0.000000 0.122813 0.000000 -0.948072 -78.310186 8.984997 -58.656865 0.000000 -19.998329 36.940994 -3.711300 -2.139901 -0.000000
*/
