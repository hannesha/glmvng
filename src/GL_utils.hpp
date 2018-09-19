#pragma once

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include <vector>

namespace GL {
// VBO RAII wrapper
class Buffer {
public:
	inline Buffer() { glGenBuffers(1, &id); };
	inline ~Buffer() { glDeleteBuffers(1, &id); };
	// disable copying
	Buffer(const Buffer&) = delete;
	Buffer(Buffer&& b): id(b.id) { b.id = 0; };
	Buffer& operator=(Buffer&&) = default;

	inline void bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); };
	inline void bind(GLenum target) const { glBindBuffer(target, id); };
	inline void operator()() const { bind(); };
	inline void operator()(GLenum target) const { bind(target); };
	inline void tfbind() const { glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, id); };

	static inline void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); };
	static inline void unbind(GLenum target) { glBindBuffer(target, 0); };
	GLuint id;
};

// VAO RAII wrapper
class VAO {
public:
	inline VAO() { glGenVertexArrays(1, &id); };
	inline ~VAO() { glDeleteVertexArrays(1, &id); };
	// disable copying
	VAO(const VAO&) = delete;
	VAO(VAO&& v): id(v.id) { v.id = 0; };
	VAO& operator=(VAO&&) = default;

	inline void bind() const { glBindVertexArray(id); };
	static inline void unbind() { glBindVertexArray(0); };
	inline void operator()() const { bind(); };
	GLuint id;
};

// Shader RAII wrapper
class Shader {
public:
	Shader(const char* code, GLuint type);
	inline ~Shader() { glDeleteShader(id); };
	// disable copying
	Shader(const Shader&) = delete;
	Shader(Shader&& s): id(s.id) { s.id = 0; };
	Shader& operator=(Shader&&) = default;

	GLuint id;
};

class Program {
public:
	Program();
	inline ~Program() { glDeleteProgram(id); };
	Program(const Program&) = delete;
	Program(Program&& p): id(p.id) { p.id = 0; };
	Program& operator=(Program&&) = default;

	template<typename ... T> void link(T& ... shs) {
		attach(shs...);
		glLinkProgram(id);
		detach(shs...);
		check_link_status();
	}

	void link_vector(const std::vector<Shader>& shs) {
		for(auto& sh : shs) {
			attach(sh);
		}

		glLinkProgram(id);

		for(auto& sh : shs) {
			detach(sh);
		}

		check_link_status();
	}

	void check_link_status();

	template<typename ... T> inline void attach(const Shader& sh, T& ... shs) { attach(sh); attach(shs...); };
	inline void attach(const Shader& sh) { glAttachShader(id, sh.id); };
	template<typename ... T> inline void detach(const Shader& sh, T& ... shs) { detach(sh); detach(shs...); };
	inline void detach(const Shader& sh) { glDetachShader(id, sh.id); };

	inline void use() const { glUseProgram(id); };
	inline void operator()() const { use(); };
	inline GLuint get_id() const { return id; };
	inline GLint get_uniform(const char* name) const { return glGetUniformLocation(id, name); };
	inline GLint get_attrib(const char* name) const { return glGetAttribLocation(id, name); };
private:
	GLuint id;
};

// Texture RAII wrapper
class Texture {
public:
	inline Texture() { glGenTextures(1, &id); };
	inline ~Texture() { glDeleteTextures(1, &id); };
	// disable copying
	Texture(const Texture&) = delete;
	// move constructor
	Texture(Texture&& t): id(t.id) { t.id = 0; };
	Texture& operator=(Texture&&) = default;

	inline void bind(GLenum target) const { glBindTexture(target, id); };
	static inline void unbind(GLenum target) { glBindTexture(target, 0); };
	inline void operator()(GLenum target) const { bind(target); };
	GLuint id;
};

// Framebuffer RAII wrapper
class FBO {
public:
	inline FBO() { glGenFramebuffers(1, &id); };
	inline ~FBO() { glDeleteFramebuffers(1, &id); };
	// disable copying
	FBO(const FBO&) = delete;
	// move constructor
	FBO(FBO&& t): id(t.id) { t.id = 0; };
	FBO& operator=(FBO&&) = default;

	inline void bind(GLenum target) const { glBindFramebuffer(target, id); };
	inline void bind() const { bind(GL_FRAMEBUFFER); };
	static inline void unbind(GLenum target) { glBindFramebuffer(target, 0); };
	static inline void unbind() { unbind(GL_FRAMEBUFFER); };
	inline void operator()(GLenum target) const { bind(target); };
	inline void operator()() const { bind(); };
	GLuint id;
};

// print error message
void get_error(const char*);
}
