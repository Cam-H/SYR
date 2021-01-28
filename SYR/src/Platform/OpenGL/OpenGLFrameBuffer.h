#pragma once

#include "SYR/Renderer/FrameBuffer.h"

namespace SYR {

	class OpenGLFrameBuffer : public FrameBuffer {
	public:

		OpenGLFrameBuffer(const FrameBufferSpecification& spec);
		virtual ~OpenGLFrameBuffer();

		virtual const FrameBufferSpecification& getSpecification() const override { return m_Specification; }
		virtual FrameBufferSpecification& getSpecification() override { return m_Specification; }

		void invalidate();

		void bind() override;
		void unbind() override;

		virtual void resize(uint32_t width, uint32_t height) override;

		virtual uint32_t getColorAttachmentRendererID() const override { return m_ColorAttachment; }

	private:
		uint32_t m_RendererID;
		uint32_t m_ColorAttachment, m_DepthAttachment;

		FrameBufferSpecification m_Specification;
	};

}