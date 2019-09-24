#include "./bvh-parser.hpp"

const std::string example = ("HIERARCHY\nROOT pelvis\n{\n	OFFSET 0.000000 0.000000 0.000000\n	CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n	JOINT lfemur\n	{\n		OFFSET 11.446000 0.000000 0.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT ltibia\n		{\n			OFFSET 0.000000 -42.030400 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT lfoot\n			{\n				OFFSET 0.000000 -39.930200 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT ltoes\n				{\n					OFFSET 0.000000 0.000000 11.775800\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					End Site\n					{\n					    OFFSET 0.000000 0.000000 5.000000\n					}\n				}\n			}\n		}\n	}\n	JOINT rfemur\n	{\n		OFFSET -11.446000 0.000000 0.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT rtibia\n		{\n			OFFSET 0.000000 -42.030400 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT rfoot\n			{\n				OFFSET 0.000000 -39.930200 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT rtoes\n				{\n					OFFSET 0.000000 0.000000 11.775800\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					End Site\n					{\n					    OFFSET 0.000000 0.000000 5.000000\n					}\n				}\n			}\n		}\n	}\n	JOINT thorax\n	{\n		OFFSET 0.000000 11.255600 -5.000000\n		CHANNELS 3 Zrotation Xrotation Yrotation\n		JOINT head\n		{\n			OFFSET 0.000000 43.799300 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			End Site\n			{\n			    OFFSET 0.000000 5.000000 0.000000\n			}\n		}\n		JOINT lclavicle\n		{\n			OFFSET 0.000000 39.863000 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT lhumerus\n			{\n				OFFSET 17.380600 4.958300 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT lradius\n				{\n					OFFSET 0.000000 -27.561300 0.000000\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					JOINT lhand\n					{\n						OFFSET 0.000000 -26.933000 0.000000\n						CHANNELS 3 Zrotation Xrotation Yrotation\n						End Site\n						{\n						    OFFSET 0.000000 -5.000000 0.000000\n						}\n					}\n				}\n			}\n		}\n		JOINT rclavicle\n		{\n			OFFSET 0.000000 39.863000 0.000000\n			CHANNELS 3 Zrotation Xrotation Yrotation\n			JOINT rhumerus\n			{\n				OFFSET -17.380600 4.958300 0.000000\n				CHANNELS 3 Zrotation Xrotation Yrotation\n				JOINT rradius\n				{\n					OFFSET 0.000000 -27.561300 0.000000\n					CHANNELS 3 Zrotation Xrotation Yrotation\n					JOINT rhand\n					{\n						OFFSET 0.000000 -26.933000 0.000000\n						CHANNELS 3 Zrotation Xrotation Yrotation\n						End Site\n						{\n						    OFFSET 0.000000 -5.000000 0.000000\n						}\n					}\n				}\n			}\n		}\n	}\n}\nMOTION\nFrames: 3\nFrame Time: 0.008342\n59.293900 87.796900 4.760200 -0.467276 -1.378122 -89.563821 -5.641307 2.819435 -8.651036 -0.000000 9.240240 -0.000000 0.000000 3.555270 6.177856 0.000000 -1.301336 -0.000000 -1.803037 2.742629 -3.741876 -0.000000 9.274983 -0.000000 0.000000 3.290364 2.906258 0.000000 -29.528157 0.000000 -4.531867 0.132349 0.213401 15.628959 24.623904 -0.608822 0.394562 0.000000 3.487028 83.769402 0.553653 62.001030 -0.000001 -12.781487 -55.037055 5.615823 0.703335 0.000000 0.095169 -0.000000 -0.923419 -78.312883 9.001808 -58.564139 0.000000 -20.003803 36.764996 -3.936911 -2.063559 -0.000000 \n59.291700 87.793600 4.752400 -0.475906 -1.381233 -89.556452 -5.638376 2.830551 -8.625235 0.000000 9.236131 -0.000000 -0.000000 3.523541 6.156061 0.000000 -1.301682 -0.000000 -1.792531 2.748974 -3.691834 -0.000000 9.276495 0.000000 0.000000 3.262773 2.860635 0.000000 -29.527798 0.000000 -4.547519 0.143945 0.207558 15.646355 24.636287 -0.589535 0.469632 -0.000000 3.531537 83.641881 0.487220 62.085857 -0.000000 -12.806394 -55.155542 5.630933 0.721572 0.000000 0.133438 -0.000000 -0.934718 -78.356085 8.980916 -58.621133 0.000000 -19.969767 36.707936 -3.798525 -2.110204 -0.000000");

using namespace inyong_bvh;

std::shared_ptr<Bvh> BvhParser::parse_from_tokens(const std::deque<BvhToken>& tokens) {
    std::deque<BvhToken> tokens_for_consume;
    tokens_for_consume.assign(tokens.begin(), tokens.end());
    hierarchy(tokens_for_consume);
    motion(tokens_for_consume);
    return nullptr;
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
