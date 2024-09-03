#include "pch.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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
    const uint32_t cMeshImportFlags =
      aiProcess_CalcTangentSpace |
      aiProcess_Triangulate |
      aiProcess_SortByPType |
      aiProcess_GenNormals |
      aiProcess_GenUVCoords |
      aiProcess_OptimizeMeshes |
      aiProcess_ValidateDataStructure;

    Assimp::Importer Importer;

    const aiScene* pScene = Importer.ReadFile(filepath.c_str(),
      cMeshImportFlags);

    if (pScene && pScene->HasMeshes())
    {
      mVertexIndices.clear();
      mVertices.clear();

      auto* mesh = pScene->mMeshes[0];

      uint32_t sumVertexIndecies = 0;
      uint32_t prevSumVertexIndecies = 0;
      
      // Vertecies of a unit cube
      // TODO: This should be a const probably
      #define UNIT_CUBE_NUM_VERTECIES 8U
      #define UNIT_CUBE_NUM_FACES     12U
      std::array<VertexHolder, UNIT_CUBE_NUM_VERTECIES> unitCubeVertecies({
          VertexHolder{ glm::vec3{-1, -1, -1}, glm::vec3{-1, -1, -1} },
          VertexHolder{ glm::vec3{-1, -1,  1}, glm::vec3{-1, -1,  1} },
          VertexHolder{ glm::vec3{-1,  1, -1}, glm::vec3{-1,  1, -1} },
          VertexHolder{ glm::vec3{-1,  1,  1}, glm::vec3{-1,  1,  1} },
          VertexHolder{ glm::vec3{ 1, -1, -1}, glm::vec3{ 1, -1, -1} },
          VertexHolder{ glm::vec3{ 1, -1,  1}, glm::vec3{ 1, -1,  1} },
          VertexHolder{ glm::vec3{ 1,  1, -1}, glm::vec3{ 1,  1, -1} },
          VertexHolder{ glm::vec3{ 1,  1,  1}, glm::vec3{ 1,  1,  1} }
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

      // Draw unit cube
      {
          // Add vertecies
          for (const auto& vertex : unitCubeVertecies)
          {
              add_vertex(vertex);
          }

          // Add faces
          for (const auto& face : unitCubeFaces)
          {
              for (auto index : face)
              {
                  add_vertex_index(index);
              }
          }
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