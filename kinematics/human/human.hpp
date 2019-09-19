#include <memory>

#include "../link/link.hpp"
#include "../joint/joint.hpp"

class Human : public Drawable {
public:
    std::shared_ptr<Link> hips;

    std::shared_ptr<Joint> right_hip_joint;
    std::shared_ptr<Link> right_thigh_link;
    std::shared_ptr<Joint> right_knee_joint;
    std::shared_ptr<Link> right_calf_link;
    std::shared_ptr<Joint> right_ankle_joint;
    std::shared_ptr<Link> right_foot_link;

    std::shared_ptr<Joint> left_hip_joint;
    std::shared_ptr<Link> left_thigh_link;
    std::shared_ptr<Joint> left_knee_joint;
    std::shared_ptr<Link> left_calf_link;
    std::shared_ptr<Joint> left_ankle_joint;
    std::shared_ptr<Link> left_foot_link;

    std::shared_ptr<Joint> sacrum_joint;
    std::shared_ptr<Link> waist_link;
    std::shared_ptr<Joint> backbone_joint;
    std::shared_ptr<Link> backbone_link;

    std::shared_ptr<Joint> right_shoulder_joint;
    std::shared_ptr<Link> right_upper_arm_link;
    std::shared_ptr<Joint> right_elbow_joint;
    std::shared_ptr<Link> right_lower_arm_link;
    std::shared_ptr<Joint> right_wrist_joint;
    std::shared_ptr<Link> right_hand_link;

    std::shared_ptr<Joint> left_shoulder_joint;
    std::shared_ptr<Link> left_upper_arm_link;
    std::shared_ptr<Joint> left_elbow_joint;
    std::shared_ptr<Link> left_lower_arm_link;
    std::shared_ptr<Joint> left_wrist_joint;
    std::shared_ptr<Link> left_hand_link;

    std::shared_ptr<Joint> neck_joint;
    std::shared_ptr<Link> neck_link;
    std::shared_ptr<Joint> head_joint;
    std::shared_ptr<Link> head_link;
    /*
    Hips (Link) (root)
      right hip joint
        thigh link
          knee joint
            calf link
              ankle joint
                foot link
      left hip joint
        thigh link
          knee joint
            calf link
              ankle joint
                foot link
      sacrum joint
        waist link
          backbone joint
            backbone link
              right shoulder joint
                upper arm link
                  elbow joint
                    lower arm link
                      wrist joint
                        hand link
              left shoulder joint
                upper arm link
                  elbow joint
                    lower arm link
                      wrist joint
                        hand link
              neck joint
                neck link
                  head joint
                    head link
    */
};

class OpenGLHuman : public Human {
public:
    OpenGLHuman();

    virtual void draw();
};
