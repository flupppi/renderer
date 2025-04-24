module;
#pragma once
#include <glm/ext/quaternion_float.hpp>
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <tiny_gltf.h>


export module BillboardCloud;
import std;
import GameInterface;
import IBillboardGenerator;
import IPlaneSelector;
import BillboardRenderer;
import InputSystem;
import Camera;
import Quad;
import ShaderUtil;
import Model;
import Plane;
import Mesh;
import Face;
import IRenderer;

namespace Engine {
    export class BillboardCloud : public GameInterface {
    public:
        BillboardCloud(std::unique_ptr<IBillboardGenerator> generator,
                       std::unique_ptr<IPlaneSelector> selector, std::filesystem::path modelPath)
            : m_billboardGenerator(std::move(generator)),
              m_planeSelector(std::move(selector)), m_modelPath(modelPath) {
        }

        void Initialize(GLFWwindow *window) override;

        void Render(float aspectRatio) override;

        void ClearResources() override {
            m_renderer.ClearResources();
        }

        void Update(double deltaTime) override;

        void RenderIMGui();

    private:
        BillboardRenderer m_renderer;
        InputSystem m_input;
        Camera m_camera;
        std::vector<Quad> m_scene;
        const std::unique_ptr<IBillboardGenerator> m_billboardGenerator;
        const std::unique_ptr<IPlaneSelector> m_planeSelector;
        const std::filesystem::path m_modelPath;
        tinygltf::Model m_model;
        std::vector<VaoRange> meshToVertexArrays;

        std::vector<GLuint> m_bufferObjects;
        std::vector<GLuint> m_vertexArrayObjects;

        Plane generatePlaneFromParams(float d, float theta, float phi);

        bool loadGltfFile(tinygltf::Model &model);

        float computeDensityForPlane(const Plane &plane, const Mesh &mesh, float epsilon);

        bool isPlaneValidForFace(const Plane &plane, const Face &face, const Mesh &mesh, float epsilon);

        float calculateProjectedArea(const Plane &plane, const Face &face, const Mesh &mesh);

        Plane m_currentPlane;
        float m_d{0.0f};
        float m_theta{0.0f};
        float m_phi{0.0f};
        float m_density{0.0f};
        float m_penalty{0.0f};
    };


    bool BillboardCloud::loadGltfFile(tinygltf::Model &model) {
        std::clog << "Loading gltf file: " << m_modelPath << std::endl;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;
        bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, m_modelPath.string());
        if (!warn.empty()) {
            std::cerr << warn << std::endl;
        }
        if (!err.empty()) {
            std::cerr << err << std::endl;
        }

        if (!ret) {
            std::cerr << "Failed to parse glTF file" << std::endl;
            return false;
        }
        return true;
    }


    float BillboardCloud::computeDensityForPlane(const Plane &plane, const Mesh &mesh, float epsilon) {
        float density = 0.0f;
        std::vector<Face> faces = mesh.ExtractFaces();
        for (const Face &face: faces) {
            if (isPlaneValidForFace(plane, face, mesh, epsilon)) {
                float projectedArea = calculateProjectedArea(plane, face, mesh);
                std::cout << "Projected Area: " << projectedArea << std::endl;
                density += projectedArea;
            }
        }

        return density;
    }

    bool BillboardCloud::isPlaneValidForFace(const Plane &plane, const Face &face, const Mesh &mesh, float epsilon) {
        // Retrieve vertex positions from the mesh using face indices
        glm::vec3 v0 = mesh.vertices[face.v0].position;
        glm::vec3 v1 = mesh.vertices[face.v1].position;
        glm::vec3 v2 = mesh.vertices[face.v2].position;

        // Check distance for each vertex
        std::array<glm::vec3, 3> vertices = {v0, v1, v2};
        for (const auto &vertex: vertices) {
            float distance = glm::abs(glm::dot(plane.normal, vertex) + plane.originOffset);
            if (distance > epsilon) {
                return false; // Plane is not valid for this face
            }
        }

        return true; // All vertices are within the error threshold
    }

    float BillboardCloud::calculateProjectedArea(const Plane &plane, const Face &face, const Mesh &mesh) {
        // Retrieve vertex positions from the mesh using face indices
        glm::vec3 v0 = mesh.vertices[face.v0].position;
        glm::vec3 v1 = mesh.vertices[face.v1].position;
        glm::vec3 v2 = mesh.vertices[face.v2].position;

        // Compute face normal and area
        glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        float faceArea = 0.5f * glm::length(glm::cross(v1 - v0, v2 - v0));

        // Projected area onto the plane
        return faceArea * glm::abs(glm::dot(faceNormal, plane.normal));
    }

    void BillboardCloud::RenderIMGui() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame(); {
            ImGui::Begin("Game HUD");
            ImGui::Text("Render Mode: %s", m_camera.DebugMode().c_str());
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }
        ImGui::End();
        ImGui::Begin("Plane Space Metrics");

        // Input sliders
        ImGui::SliderFloat("Distance (d)", &m_d, -10.0f, 10.0f);
        ImGui::SliderFloat("Azimuth (theta)", &m_theta, 0.0f, 360.0f);
        ImGui::SliderFloat("Polar Angle (phi)", &m_phi, 0.0f, 180.0f);


        ImGui::Text("Plane Metrics:");
        ImGui::Text("Density: %.3f", m_density);
        ImGui::Text("Penalty: %.3f", m_penalty);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    Plane BillboardCloud::generatePlaneFromParams(float d, float theta, float phi) {
        glm::vec3 normal = glm::normalize(glm::vec3(
            std::sin(glm::radians(phi)) * std::cos(glm::radians(theta)),
            std::sin(glm::radians(phi)) * std::sin(glm::radians(theta)),
            std::cos(glm::radians(phi))
        ));
        return {normal, d};
    }

    glm::mat4 getLocalToWorldMatrix(
        const tinygltf::Node &node, const glm::mat4 &parentMatrix)
    {
        // Extract model matrix
        // https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#transformations
        if (!node.matrix.empty()) {
            return parentMatrix * glm::mat4(node.matrix[0], node.matrix[1],
                                      node.matrix[2], node.matrix[3], node.matrix[4],
                                      node.matrix[5], node.matrix[6], node.matrix[7],
                                      node.matrix[8], node.matrix[9], node.matrix[10],
                                      node.matrix[11], node.matrix[12], node.matrix[13],
                                      node.matrix[14], node.matrix[15]);
        }
        const auto T = node.translation.empty()
                           ? parentMatrix
                           : glm::translate(parentMatrix,
                                 glm::vec3(node.translation[0], node.translation[1],
                                     node.translation[2]));
        const auto rotationQuat =
            node.rotation.empty()
                ? glm::quat(1, 0, 0, 0)
                : glm::quat(float(node.rotation[3]), float(node.rotation[0]),
                      float(node.rotation[1]),
                      float(node.rotation[2])); // prototype is w, x, y, z
        const auto TR = T * glm::mat4_cast(rotationQuat);
        return node.scale.empty() ? TR
                                  : glm::scale(TR, glm::vec3(node.scale[0],
                                                       node.scale[1], node.scale[2]));
    };


    void BillboardCloud::Render(float aspectRatio) {
        glm::mat4 Projection{m_camera.GetProjectionMatrix(aspectRatio)};
        glm::mat4 View{m_camera.GetViewMatrix()};
        // Lambda function to draw the scene
        const auto drawScene = [&](const Camera &camera) {
            const glm::mat4 projMatrix{camera.GetProjectionMatrix(aspectRatio)};

            const auto viewMatrix = m_camera.GetViewMatrix();

            // The recursive function that should draw a node
            // We use a std::function because a simple lambda cannot be recursive
            const std::function<void(int, const glm::mat4 &)> drawNode =
                    [&](int nodeIdx, const glm::mat4 &parentMatrix) {
                const auto &node = m_model.nodes[nodeIdx];
                const glm::mat4 modelMatrix =
                        getLocalToWorldMatrix(node, parentMatrix);

                // If the node references a mesh (a node can also reference a
                // camera, or a light)
                if (node.mesh >= 0) {
                    const auto mvMatrix =
                            viewMatrix * modelMatrix; // Also called localToCamera matrix
                    const auto mvpMatrix = projMatrix * mvMatrix; // Also called localToScreen matrix
                    // Normal matrix is necessary to maintain normal vectors
                    // orthogonal to tangent vectors
                    // https://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/
                    const auto normalMatrix = glm::transpose(glm::inverse(mvMatrix));

                    m_renderer.m_glslProgram->setMat4("uModelViewProjMatrix",
                                       mvpMatrix);
                    if (m_renderer.modelViewMatrixLocation > 0)
                    m_renderer.m_glslProgram->setMat4("uModelViewMatrix", mvMatrix);
                    m_renderer.m_glslProgram->setMat4("uNormalMatrix", normalMatrix);

                    const auto &mesh = m_model.meshes[node.mesh];
                    const auto &vaoRange = meshToVertexArrays[node.mesh];
                    for (size_t pIdx = 0; pIdx < mesh.primitives.size(); ++pIdx) {
                        const auto vao = m_vertexArrayObjects[vaoRange.begin + pIdx];
                        const auto &primitive = mesh.primitives[pIdx];
                        glBindVertexArray(vao);
                        if (primitive.indices >= 0) {
                            const auto &accessor = m_model.accessors[primitive.indices];
                            const auto &bufferView = m_model.bufferViews[accessor.bufferView];
                            const auto byteOffset =
                                    accessor.byteOffset + bufferView.byteOffset;
                            glDrawElements(primitive.mode, GLsizei(accessor.count),
                                           accessor.componentType, (const GLvoid *) byteOffset);
                        } else {
                            // Take first accessor to get the count
                            const auto accessorIdx = (*begin(primitive.attributes)).second;
                            const auto &accessor = m_model.accessors[accessorIdx];
                            glDrawArrays(primitive.mode, 0, GLsizei(accessor.count));
                        }
                    }
                }

                // Draw children
                for (const auto childNodeIdx: node.children) {
                    drawNode(childNodeIdx, modelMatrix);
                }
            };

            // Draw the scene referenced by gltf file
            if (m_model.defaultScene >= 0) {
                for (const auto nodeIdx: m_model.scenes[m_model.defaultScene].nodes) {
                    drawNode(nodeIdx, glm::mat4(1));
                }
            }
        };

        // Use the transform matrix from the TransformComponent
        for (auto model: m_scene) {
            glm::mat4 Model{model.trans.GetTransform()};

            glm::mat4 mvp{Projection * View * Model};
            m_renderer.RenderQuad(mvp);
        }

        glm::mat4 Model{glm::mat4(1.0f)};
        glm::mat4 transformation_model = Projection * View * Model;
        m_renderer.m_glslProgram->use();
        drawScene(m_camera);


        glm::mat4 mvp_plane = Projection * View * Model;
        m_renderer.RenderPlane(m_currentPlane, mvp_plane); // Delegate drawing to the renderer


        // Render the gizmo lines
        m_renderer.RenderGizmo(Projection * View * glm::scale(Model, glm::vec3(0.5f)));
        RenderIMGui();
    }

    void BillboardCloud::Initialize(GLFWwindow *window) {
        m_input.SetWindow(window);
        // Observe Input
        m_input.ObserveKey(GLFW_KEY_SPACE);
        m_input.ObserveKey(GLFW_KEY_RIGHT);
        m_input.ObserveKey(GLFW_KEY_LEFT);
        m_input.ObserveKey(GLFW_KEY_UP);
        m_input.ObserveKey(GLFW_KEY_DOWN);
        m_input.ObserveKey(GLFW_KEY_9);
        m_input.ObserveKey(GLFW_KEY_8);
        m_input.ObserveKey(GLFW_KEY_7);
        m_input.ObserveKey(GLFW_KEY_6);
        m_input.ObserveKey(GLFW_KEY_4);
        m_input.ObserveKey(GLFW_KEY_3);
        m_input.ObserveKey(GLFW_KEY_2);
        m_input.ObserveKey(GLFW_KEY_1);
        m_input.ObserveKey(GLFW_KEY_R);
        m_input.ObserveKey(GLFW_KEY_LEFT_SHIFT);
        m_input.ObserveKey(GLFW_KEY_J);
        m_input.ObserveKey(GLFW_KEY_K);
        m_input.ObserveKey(GLFW_KEY_I);
        m_input.ObserveKey(GLFW_KEY_M);

        // Movement Keys
        m_input.ObserveKey(GLFW_KEY_W);
        m_input.ObserveKey(GLFW_KEY_A);
        m_input.ObserveKey(GLFW_KEY_S);
        m_input.ObserveKey(GLFW_KEY_D);
        m_input.ObserveKey(GLFW_KEY_Q);
        m_input.ObserveKey(GLFW_KEY_E);
        m_input.ObserveKey(GLFW_KEY_LEFT_ALT);

        m_renderer.Initialize();
        // Update plane
        m_currentPlane = Plane(m_d, m_theta, m_phi);
        m_currentPlane.setupMesh(); // Generate vertices and OpenGL buffers
        auto vertexes = std::vector<Vertex>{
            Vertex(glm::vec3(0.0f, 0.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 1.0f, 0.0f)),
            Vertex(glm::vec3(0.0f, 0.0f, 1.0f))
        };
        auto indexes = std::vector<unsigned int>{0, 1, 2};
        // Triangle face} };
        Mesh testMesh{vertexes, indexes, {}};
        float epsilon = 0.1f;
        float density = computeDensityForPlane(m_currentPlane, testMesh, epsilon);
        std::cout << "Density: " << density << std::endl;
        // Define Objects in the scene.
        //for (int i = 0; i < 5; i++) {
        //	Quad quad;
        //	quad.trans.SetPosition(glm::vec3(-5.0f + (i * 2.1f), 0.0f, 0.0f));
        //	m_scene.push_back(quad);
        //	m_renderer.InitQuad(quad);
        //}
        if (!loadGltfFile(m_model)) {
            throw std::runtime_error("Failed to load gltf file");
        }
        // TODO Creation of Buffer Objects
        m_bufferObjects = m_renderer.createBufferObjects(m_model);

        // TODO Creation of Vertex Array Objects
        m_vertexArrayObjects = m_renderer.createVertexArrayObjects(m_model, m_bufferObjects, meshToVertexArrays);
    }

    void BillboardCloud::Update(double deltaTime) {
        m_input.Update();
        bool rotateLeft{m_input.IsKeyDown(GLFW_KEY_LEFT)};
        bool rotateRight{m_input.IsKeyDown(GLFW_KEY_RIGHT)};
        bool zoomIn{m_input.IsKeyDown(GLFW_KEY_UP)};
        bool zoomOut{m_input.IsKeyDown(GLFW_KEY_DOWN)};

        bool right{m_input.IsKeyDown(GLFW_KEY_K)};
        bool left{m_input.IsKeyDown(GLFW_KEY_J)};
        bool up{m_input.IsKeyDown(GLFW_KEY_I)};
        bool down{m_input.IsKeyDown(GLFW_KEY_M)};
        if (right) {
            glm::vec3 deltaPos{1.0f * deltaTime, 0.0f, 0.0f};
            glm::vec3 oldPos{m_scene.at(0).trans.GetPosition()};
            glm::vec3 newPos{oldPos + deltaPos};
            m_scene.at(0).trans.SetPosition(newPos);
        }
        if (left) {
            glm::vec3 deltaPos{-1.0f * deltaTime, 0.0f, 0.0f};
            glm::vec3 oldPos{m_scene.at(0).trans.GetPosition()};
            glm::vec3 newPos{oldPos + deltaPos};
            m_scene.at(0).trans.SetPosition(newPos);
        }
        if (up) {
            glm::vec3 deltaPos{0.0f, 1.0f * deltaTime, 0.0f};
            glm::vec3 oldPos{m_scene.at(0).trans.GetPosition()};
            glm::vec3 newPos{oldPos + deltaPos};
            m_scene.at(0).trans.SetPosition(newPos);
        }
        if (down) {
            glm::vec3 deltaPos{0.0f, -1.0f * deltaTime, 0.0f};
            glm::vec3 oldPos{m_scene.at(0).trans.GetPosition()};
            glm::vec3 newPos{oldPos + deltaPos};
            m_scene.at(0).trans.SetPosition(newPos);
        }
        if (m_input.WasKeyPressed(GLFW_KEY_LEFT_ALT)) {
            m_camera.ToggleMode();
        }


        if (m_camera.GetCameraMode() == Camera::CameraMode::FPS) {
            if (m_input.IsRightMouseButtonDown())
                m_camera.UpdateDirection(m_input.GetMouseDeltaX(), m_input.GetMouseDeltaY());
            else
                m_camera.UpdateDirection(0, 0);
        }

        m_camera.HandleInput(deltaTime,
                             m_input.IsKeyDown(GLFW_KEY_W), m_input.IsKeyDown(GLFW_KEY_S),
                             m_input.IsKeyDown(GLFW_KEY_A), m_input.IsKeyDown(GLFW_KEY_D),
                             m_input.IsKeyDown(GLFW_KEY_Q), m_input.IsKeyDown(GLFW_KEY_E),
                             rotateLeft, rotateRight,
                             zoomIn, zoomOut);

        // Update the existing plane
        m_currentPlane.UpdateTransform(m_d, m_theta, m_phi);


        // Update the camera with input flags
        m_camera.Update(deltaTime, rotateLeft, rotateRight, zoomIn, zoomOut);
    }
}
