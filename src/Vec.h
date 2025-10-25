#pragma once
#include "Includes.h"
#include "./MemoryHandler/Offsets.h"
#include "Globals.h"

namespace Vec
{
    struct Vector3
    {
        float x, y, z;
    };

    struct ScreenPoint
    {
        float x, y, w;
        ScreenPoint() : x(0), y(0), w(0) {} // Default constructor
        ScreenPoint(float x, float y) : x(x), y(y) {} // custom
    };

    struct Size
    {
        int Width, Height;
    };

    struct Matrix4x4 {
        float _11, _12, _13, _14;
        float _21, _22, _23, _24;
        float _31, _32, _33, _34;
        float _41, _42, _43, _44;
    };

    struct ViewMatrix
    {
        float M11, M12, M13, M14;
        float M21, M22, M23, M24;
        float M31, M32, M33, M34;
        float M41, M42, M43, M44;

        ViewMatrix(const std::vector<float>& elements)
        {
            if (elements.size() != 16)
            {
                throw std::invalid_argument("The vector must contain exactly 16 float values.");
            }

            M11 = elements[0]; M12 = elements[1]; M13 = elements[2]; M14 = elements[3];
            M21 = elements[4]; M22 = elements[5]; M23 = elements[6]; M24 = elements[7];
            M31 = elements[8]; M32 = elements[9]; M33 = elements[10]; M34 = elements[11];
            M41 = elements[12]; M42 = elements[13]; M43 = elements[14]; M44 = elements[15];
        }

        ViewMatrix() = default;

        // Add array constructor (more efficient)
        ViewMatrix(const float* elements)
        {
            M11 = elements[0]; M12 = elements[1]; M13 = elements[2]; M14 = elements[3];
            M21 = elements[4]; M22 = elements[5]; M23 = elements[6]; M24 = elements[7];
            M31 = elements[8]; M32 = elements[9]; M33 = elements[10]; M34 = elements[11];
            M41 = elements[12]; M42 = elements[13]; M43 = elements[14]; M44 = elements[15];
        }

        inline static const ViewMatrix Identity()
        {
            return ViewMatrix(std::vector<float>{
                1, 0, 0, 0,
                    0, 1, 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            });
        }

        ViewMatrix Transpose() const
        {
            return ViewMatrix(std::vector<float>{
                M11, M21, M31, M41,
                    M12, M22, M32, M42,
                    M13, M23, M33, M43,
                    M14, M24, M34, M44
            });
        }

        ViewMatrix BuildLegacyGameMatrix() const
        {
            return ViewMatrix(std::vector<float>{
                this->M11, this->M21, this->M31, this->M41,
                    -this->M12, -this->M22, -this->M32, -this->M42,
                    this->M13, this->M23, this->M33, this->M43,
                    this->M14, this->M24, this->M34, this->M44
            });
        }

        std::string ToString() const
        {
            std::ostringstream sb;
            sb << std::fixed << std::setprecision(2); // Set precision for float output
            sb << "[" << M11 << ", " << M12 << ", " << M13 << ", " << M14 << "]\n";
            sb << "[" << M21 << ", " << M22 << ", " << M23 << ", " << M24 << "]\n";
            sb << "[" << M31 << ", " << M32 << ", " << M33 << ", " << M34 << "]\n";
            sb << "[" << M41 << ", " << M42 << ", " << M43 << ", " << M44 << "]\n";
            return sb.str();
        }
    };

    inline bool WorldToScreen(const Vector3 worldPos, const ViewMatrix& rawViewMatrix, const Size screenSize, ScreenPoint& screenCoords)
    {
        const ViewMatrix gameMatrix = rawViewMatrix.BuildLegacyGameMatrix();

        float sx = gameMatrix.M11 * worldPos.x + gameMatrix.M12 * worldPos.y + gameMatrix.M13 * worldPos.z + gameMatrix.M14;
        float sy = gameMatrix.M21 * worldPos.x + gameMatrix.M22 * worldPos.y + gameMatrix.M23 * worldPos.z + gameMatrix.M24;
        float w = gameMatrix.M41 * worldPos.x + gameMatrix.M42 * worldPos.y + gameMatrix.M43 * worldPos.z + gameMatrix.M44;

        if (w < 0.01f)
            return false;

        float invW = 1.0f / w;
        sx *= invW;
        sy *= invW;

        float x = screenSize.Width / 2.0f + 0.5f * sx * screenSize.Width + 0.5f;
        float y = screenSize.Height / 2.0f - 0.5f * sy * screenSize.Height + 0.5f;

        if (x > 0 && x < screenSize.Width && y > 0 && y < screenSize.Height)
        {
            screenCoords.x = x;
            screenCoords.y = y;
            screenCoords.w = w;
            return true;
        }

        return false;
    }


    inline ViewMatrix ReadViewMatrix()
    {
        if (g_resolvedViewMatrixAddress == 0) {
            return ViewMatrix::Identity();
        }

        float matrix[16] = {};
        SIZE_T bytesRead = 0;

        if (!ReadProcessMemory(hProc,
            reinterpret_cast<LPCVOID>(g_resolvedViewMatrixAddress),
            matrix,
            sizeof(matrix),
            &bytesRead) || bytesRead != sizeof(matrix))
        {
            return ViewMatrix::Identity();
        }

        return ViewMatrix(matrix); // Direct array construction
    }



    inline bool InitializeViewMatrixAddress(uintptr_t moduleBase)
    {
        g_resolvedViewMatrixAddress = ResolvePointer(hProc, moduleBase + Offsets::game::CGraphics, Offsets::game::viewMatrix);
        return g_resolvedViewMatrixAddress != 0;
    }
}

