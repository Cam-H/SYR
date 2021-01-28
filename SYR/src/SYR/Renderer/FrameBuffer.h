#pragma once

#include "SYR/Core/Core.h"

namespace SYR {

	struct FrameBufferSpecification {
		uint32_t width, height;
		uint32_t samples = 1;

		bool swapChainTarget = false;
	};

	class FrameBuffer {
	public:

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t getColorAttachmentRendererID() const = 0;

		virtual const FrameBufferSpecification& getSpecification() const = 0;
		virtual FrameBufferSpecification& getSpecification() = 0;

		static Ref<FrameBuffer> create(const FrameBufferSpecification& spec);
	};

}