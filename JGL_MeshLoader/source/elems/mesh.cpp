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

      // Draw marker for each data point
      uint32_t iRow = 0;
      for (csv::CSVRow& row : reader)
      {
        // Add vertecies
        glm::vec3 pos{
            row["Points_m_XYZ:0"].get<float>(),
            row["Points_m_XYZ:2"].get<float>(),
            -row["Points_m_XYZ:1"].get<float>()
        };
        add_vertex(VertexHolder{
            pos, { 0.3 + pos.y, 0.27f - pos.y, 0.5f }
        });

        add_vertex_index(iRow++);
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