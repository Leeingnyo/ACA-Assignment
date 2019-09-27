#include <vector>
#include <memory>

#include "../joint/joint.hpp"
#include "../link/link.hpp"

class RootJoint : public Joint {
private:
    void animate(int frame_index, std::shared_ptr<Link> link);
    int channel_index;
public:
    std::vector<float> animation_information;
    int number_of_channels;
    float frame_time;

    void animate(int frame_index);

    void draw();
};
