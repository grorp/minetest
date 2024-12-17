// Luanti
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "catch.h"
#include "irrMath.h"
#include "matrix4.h"
#include "irr_v3d.h"
#include "quaternion.h"

using matrix4 = core::matrix4;

static bool vec_equals(const v3f &a, const v3f &b) {
    // more tolerance than usual
    return core::equals(a.X, b.X, 0.0001f) && core::equals(a.Y, b.Y, 0.0001f) &&
            core::equals(a.Z, b.Z, 0.0001f);
}

static bool matrix_equals(const matrix4 &a, const matrix4 &b) {
    return a.equals(b, 0.00001f);
}

constexpr v3f x{1, 0, 0};
constexpr v3f y{0, 1, 0};
constexpr v3f z{0, 0, 1};

TEST_CASE("matrix4") {

SECTION("setRotationRadians") {
    SECTION("rotation order is ZYX (matrix notation)") {
        v3f rot{1, 2, 3};
        matrix4 X, Y, Z, ZYX;
        X.setRotationRadians({rot.X, 0, 0});
        Y.setRotationRadians({0, rot.Y, 0});
        Z.setRotationRadians({0, 0, rot.Z});
        ZYX.setRotationRadians(rot);
        CHECK(!matrix_equals(X * Y * Z, ZYX));
        CHECK(!matrix_equals(X * Z * Y, ZYX));
        CHECK(!matrix_equals(Y * X * Z, ZYX));
        CHECK(!matrix_equals(Y * Z * X, ZYX));
        CHECK(!matrix_equals(Z * X * Y, ZYX));
        CHECK(matrix_equals(Z * Y * X, ZYX));
    }

    const f32 quarter_turn = core::PI / 2;

    // See https://en.wikipedia.org/wiki/Right-hand_rule#/media/File:Cartesian_coordinate_system_handedness.svg
    // for a visualization of what handedness means for rotations

    SECTION("rotation is right-handed") {
        SECTION("rotation around the X-axis is Z-up, counter-clockwise") {
            matrix4 X;
            X.setRotationRadians({quarter_turn, 0, 0});
            CHECK(X.transformVect(x).equals(x));
            CHECK(X.transformVect(y).equals(z));
            CHECK(X.transformVect(z).equals(-y));
        }

        SECTION("rotation around the Y-axis is Z-up, clockwise") {
            matrix4 Y;
            Y.setRotationRadians({0, quarter_turn, 0});
            CHECK(Y.transformVect(y).equals(y));
            CHECK(Y.transformVect(x).equals(-z));
            CHECK(Y.transformVect(z).equals(x));
        }

        SECTION("rotation around the Z-axis is Y-up, counter-clockwise") {
            matrix4 Z;
            Z.setRotationRadians({0, 0, quarter_turn});
            CHECK(Z.transformVect(z).equals(z));
            CHECK(Z.transformVect(x).equals(y));
            CHECK(Z.transformVect(y).equals(-x));
        }
    }

}

SECTION("setInverseRotationRadians") {
    const std::vector<f32> floats = GENERATE(take(100,
            chunk(6, random(-100.0f, 100.0f))));
    const v3f vec{floats[0], floats[1], floats[2]};
    const v3f rot{floats[3], floats[4], floats[5]};

    // rotation matrix constructed from Euler angles directly is equal to
    // rotation matrix constructed from quaternion

    core::matrix4 rot_mat;
    rot_mat.setRotationRadians(rot);

    core::quaternion rot_quat(rot);
    core::matrix4 rot_quat_mat;
    rot_quat.getMatrixFast(rot_quat_mat);

    CHECK(matrix_equals(rot_mat, rot_quat_mat));

    // inverse rotation matrix constructed from Euler angles directly is equal
    // to rotation matrix constructed from inverse quaternion

    core::matrix4 inv_rot_mat;
    inv_rot_mat.setInverseRotationRadians(rot);

    core::quaternion inv_rot_quat(rot);
    inv_rot_quat.makeInverse();
    core::matrix4 inv_rot_quat_mat;
    inv_rot_quat.getMatrixFast(inv_rot_quat_mat);

    CHECK(matrix_equals(inv_rot_mat, inv_rot_quat_mat));

    // inverse rotation works correctly (cancels out rotation)
    CHECK(vec_equals(vec, inv_rot_mat.transformVect(rot_mat.transformVect(vec))));
}

}
