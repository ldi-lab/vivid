#pragma once

#include <glm/glm.hpp>
#include <Eigen/Dense>

namespace vivid {

class GlmUtils {
public:
    template<typename T, int m, int n>
    static glm::mat<m, n, float, glm::precision::highp> eigen2glm(const Eigen::Matrix<T, m, n> &em) {
        glm::mat<m, n, float, glm::precision::highp> mat;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                mat[j][i] = em(i, j);
            }
        }
        return mat;
    }


    template<typename T, int m, int n>
    static Eigen::Matrix<T, m, n> glm2eigen(const glm::mat<m, n, float, glm::precision::highp> &mat) {
        Eigen::Matrix<T, m, n> em;
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                em(i, j) = mat[j][i];
            }
        }
        return em;
    }

};

} // namespace vivid