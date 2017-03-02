#ifndef MI_MATH_HPP
#define MI_MATH_HPP 1
#include <Eigen/Dense>
namespace mi
{
        typedef Eigen::Vector3d Vector3d;
        typedef Eigen::Vector3f Vector3f;
        typedef Eigen::Matrix< short , 3 , 1> Vector3s;

        typedef Eigen::Vector3d Point3d;
        typedef Eigen::Vector3f Point3f;
        typedef Eigen::Vector3i Point3i;

        typedef Eigen::Vector3f Color3f;
};
#endif// MI_MATH_HPP
