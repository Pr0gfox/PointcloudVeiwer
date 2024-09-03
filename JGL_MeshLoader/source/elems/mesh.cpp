#include "pch.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <csv.hpp>

#include "render/opengl_buffer_manager.h"

#include <array>

namespace nelems
{
  void Mesh::init()
  {
    mRenderBufferMgr = std::make_unique<nrender::OpenGL_VertexIndexBuffer>();

    create_buffers();
  }

  Mesh::~Mesh()
  {
    delete_buffers();
  }

  bool Mesh::load(const std::string& filepath)
  {
    csv::CSVFormat format;
    format.delimiter(',')
        .quote('~')
        .quote(false)
        .trim({ ' ', '\"' })
        .header_row(1);
    csv::CSVReader reader(filepath, format);


    // TODO: Check necessary CSV field names as a sanity check

    // Check if there is data in the csv
    //if (!reader.empty())
    {
      mVertexIndices.clear();
      mVertices.clear();

      // Vertecies of a unit cube
      // TODO: This should be a const probably
      #define UNIT_CUBE_NUM_VERTECIES 8U
      #define UNIT_CUBE_NUM_FACES     12U
      #define MARKER_HALF_SIZE        0.02
      std::array<VertexHolder, UNIT_CUBE_NUM_VERTECIES> unitCubeVertecies({
          VertexHolder{ glm::vec3{-MARKER_HALF_SIZE, -MARKER_HALF_SIZE, -MARKER_HALF_SIZE}, glm::vec3{-MARKER_HALF_SIZE, -MARKER_HALF_SIZE, -MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{-MARKER_HALF_SIZE, -MARKER_HALF_SIZE,  MARKER_HALF_SIZE}, glm::vec3{-MARKER_HALF_SIZE, -MARKER_HALF_SIZE,  MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{-MARKER_HALF_SIZE,  MARKER_HALF_SIZE, -MARKER_HALF_SIZE}, glm::vec3{-MARKER_HALF_SIZE,  MARKER_HALF_SIZE, -MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{-MARKER_HALF_SIZE,  MARKER_HALF_SIZE,  MARKER_HALF_SIZE}, glm::vec3{-MARKER_HALF_SIZE,  MARKER_HALF_SIZE,  MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{ MARKER_HALF_SIZE, -MARKER_HALF_SIZE, -MARKER_HALF_SIZE}, glm::vec3{ MARKER_HALF_SIZE, -MARKER_HALF_SIZE, -MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{ MARKER_HALF_SIZE, -MARKER_HALF_SIZE,  MARKER_HALF_SIZE}, glm::vec3{ MARKER_HALF_SIZE, -MARKER_HALF_SIZE,  MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{ MARKER_HALF_SIZE,  MARKER_HALF_SIZE, -MARKER_HALF_SIZE}, glm::vec3{ MARKER_HALF_SIZE,  MARKER_HALF_SIZE, -MARKER_HALF_SIZE} },
          VertexHolder{ glm::vec3{ MARKER_HALF_SIZE,  MARKER_HALF_SIZE,  MARKER_HALF_SIZE}, glm::vec3{ MARKER_HALF_SIZE,  MARKER_HALF_SIZE,  MARKER_HALF_SIZE} }
      });

      using triangle_t = std::array<uint32_t, 3U>;

      std::array<triangle_t, UNIT_CUBE_NUM_FACES> unitCubeFaces({
          triangle_t{ {0,2,4} },
          triangle_t{ {2,4,6} },
          triangle_t{ {0,1,2} },
          triangle_t{ {1,2,3} },
          triangle_t{ {1,3,5} },
          triangle_t{ {3,5,7} },
          triangle_t{ {4,5,6} },
          triangle_t{ {5,6,7} },
          triangle_t{ {2,3,6} },
          triangle_t{ {3,6,7} },
          triangle_t{ {0,1,4} },
          triangle_t{ {1,4,5} }
      });

      // Draw marker for each data point
      uint32_t iRow = 0;
      for (csv::CSVRow& row : reader)
      {
        // Add vertecies
        for (const auto& vertex : unitCubeVertecies)
        {
            // Note: this part rotates the data cloud 90 degrees for the display
            add_vertex(VertexHolder{
                glm::vec3{
                    vertex.mPos.x + row["Points_m_XYZ:0"].get<float>(),
                    vertex.mPos.y + row["Points_m_XYZ:2"].get<float>(),
                    vertex.mPos.z - row["Points_m_XYZ:1"].get<float>()
                },
                vertex.mNormal
            });
        }

        // Add faces
        for (const auto& face : unitCubeFaces)
        {
            for (auto index : face)
            {
                add_vertex_index(index + iRow * UNIT_CUBE_NUM_VERTECIES);
            }
        }

        iRow++;
      }

      init();
      return true;
    }
    return false;
  }

  void Mesh::create_buffers()
  {
    mRenderBufferMgr->create_buffers(mVertices, mVertexIndices);
  }

  void Mesh::delete_buffers()
  {
    mRenderBufferMgr->delete_buffers();
  }

  void Mesh::bind()
  {
    mRenderBufferMgr->bind();
  }

  void Mesh::unbind()
  {
    mRenderBufferMgr->unbind();
  }

  void Mesh::render()
  {
    mRenderBufferMgr->draw((int) mVertexIndices.size());
  }
}