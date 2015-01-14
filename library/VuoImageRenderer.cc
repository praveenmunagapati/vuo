/**
 * @file
 * VuoImageRenderer implementation.
 *
 * @copyright Copyright © 2012–2013 Kosada Incorporated.
 * This code may be modified and distributed under the terms of the MIT License.
 * For more information, see http://vuo.org/license.
 */

#include "VuoImageRenderer.h"
#include "VuoGlContext.h"
#include "VuoGlPool.h"

#include <stdlib.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOSurface/IOSurfaceAPI.h>

#include <OpenGL/OpenGL.h>
#include <OpenGL/CGLMacro.h>
#include <OpenGL/CGLIOSurface.h>
/// @{
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
/// @}

extern "C"
{
#include "module.h"

#ifdef VUO_COMPILER
VuoModuleMetadata({
					 "title" : "VuoImageRenderer",
					 "dependencies" : [
						 "VuoGlContext",
						 "VuoGlPool",
						 "OpenGL.framework"
					 ]
				 });
#endif
}

/**
 * Internal state data for a VuoImageRenderer instance.
 */
struct VuoImageRendererInternal
{
	VuoGlContext glContext;

	GLuint outputFramebuffer;

	GLuint vertexArray;
	GLuint quadDataBuffer;
	GLuint quadElementBuffer;
};

static const GLfloat quadData[] = {
	// Positions
	-1, -1, 0, 1,
	 1, -1, 0, 1,
	-1,  1, 0, 1,
	 1,  1, 0, 1,

	// Texture Coordinates
	0, 0, 0, 0,
	1, 0, 0, 0,
	0, 1, 0, 0,
	1, 1, 0, 0
};
static const GLushort quadElements[] = { 0, 1, 2, 3 };
static const GLfloat unityMatrix[16] = {
	1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1
};

void VuoImageRenderer_destroy(VuoImageRenderer ir);

/**
 * Creates a reference-counted object for rendering a @ref VuoImage.
 *
 * @threadAny
 */
VuoImageRenderer VuoImageRenderer_make(VuoGlContext glContext)
{
	struct VuoImageRendererInternal *imageRenderer = (struct VuoImageRendererInternal *)malloc(sizeof(struct VuoImageRendererInternal));
	VuoRegister(imageRenderer, VuoImageRenderer_destroy);

	imageRenderer->glContext = glContext;
	CGLContextObj cgl_ctx = (CGLContextObj)glContext;

	glGenVertexArrays(1, &imageRenderer->vertexArray);
	glBindVertexArray(imageRenderer->vertexArray);
	{
		imageRenderer->quadDataBuffer = VuoGlPool_use(glContext, VuoGlPool_ArrayBuffer, sizeof(quadData));
		glBindBuffer(GL_ARRAY_BUFFER, imageRenderer->quadDataBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STREAM_DRAW);
/// @todo https://b33p.net/kosada/node/6901
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadData), quadData);

		imageRenderer->quadElementBuffer = VuoGlPool_use(glContext, VuoGlPool_ElementArrayBuffer, sizeof(quadElements));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imageRenderer->quadElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadElements), quadElements, GL_STREAM_DRAW);
/// @todo https://b33p.net/kosada/node/6901
//		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(quadElements), quadElements);
	}
	glBindVertexArray(0);

	/// @todo https://b33p.net/kosada/node/6920
	VuoGlContext_disuse(VuoGlContext_use());

	glGenFramebuffers(1, &imageRenderer->outputFramebuffer);

	return (VuoImageRenderer)imageRenderer;
}

/**
 * Produces a new @c VuoImage by rendering @c shader.
 *
 * @threadAnyGL
 * (Additionally, the caller is responsible for ensuring that the same @c VuoImageRenderer is not used simultaneously on multiple threads.)
 */
VuoImage VuoImageRenderer_draw(VuoImageRenderer ir, VuoShader shader, unsigned int pixelsWide, unsigned int pixelsHigh)
{
	return VuoImage_make(VuoImageRenderer_draw_internal(ir,shader,pixelsWide,pixelsHigh,false), GL_RGBA, pixelsWide, pixelsHigh);
}

/**
 * Helper for VuoImageRenderer_draw().
 */
unsigned long int VuoImageRenderer_draw_internal(VuoImageRenderer ir, VuoShader shader, unsigned int pixelsWide, unsigned int pixelsHigh, bool outputToIOSurface)
{
	struct VuoImageRendererInternal *imageRenderer = (struct VuoImageRendererInternal *)ir;

	GLuint outputTexture;
	IOSurfaceID surfID;
	{
		CGLContextObj cgl_ctx = (CGLContextObj)imageRenderer->glContext;

		glViewport(0, 0, pixelsWide, pixelsHigh);

		// Create a new GL Texture Object.
		GLuint textureTarget = outputToIOSurface ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;

		if (outputToIOSurface)
		{
			glGenTextures(1, &outputTexture);
			glEnable(GL_TEXTURE_RECTANGLE_ARB);
			glBindTexture(textureTarget, outputTexture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

			glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			CFMutableDictionaryRef properties = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, NULL, NULL);
			CFDictionaryAddValue(properties, kIOSurfaceIsGlobal, kCFBooleanTrue);
			long long pixelsWideLL = pixelsWide;
			CFDictionaryAddValue(properties, kIOSurfaceWidth, CFNumberCreate(NULL, kCFNumberLongLongType, &pixelsWideLL));
			long long pixelsHighLL = pixelsHigh;
			CFDictionaryAddValue(properties, kIOSurfaceHeight, CFNumberCreate(NULL, kCFNumberLongLongType, &pixelsHighLL));
			long long bytesPerElement = 4;
			CFDictionaryAddValue(properties, kIOSurfaceBytesPerElement, CFNumberCreate(NULL, kCFNumberLongLongType, &bytesPerElement));

			IOSurfaceRef surf = IOSurfaceCreate(properties);
			CGLError err = CGLTexImageIOSurface2D(cgl_ctx, textureTarget, GL_RGB, (GLsizei)pixelsWide, (GLsizei)pixelsHigh, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, surf, 0);
			surfID = IOSurfaceGetID(surf);
			// IOSurfaceDecrementUseCount(surf); ?
			CFRelease(surf);
			if(err != kCGLNoError)
			{
				fprintf(stderr,"VuoImageRenderer_draw_internal() Error in CGLTexImageIOSurface2D(): %s\n", CGLErrorString(err));
				return 0;
			}

			glBindTexture(textureTarget, 0);
		}
		else
			outputTexture = VuoGlTexturePool_use(imageRenderer->glContext, GL_RGBA, pixelsWide, pixelsHigh, GL_RGBA);

		glBindFramebuffer(GL_FRAMEBUFFER, imageRenderer->outputFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, outputTexture, 0);

		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Execute the shader.
		{
			glUseProgram(shader->glProgramName);
			{
				VuoShader_activateTextures(shader, cgl_ctx);
				{
					glBindVertexArray(imageRenderer->vertexArray);

					GLint projectionMatrixUniform = glGetUniformLocation(shader->glProgramName, "projectionMatrix");
					glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, unityMatrix);

					GLint modelviewMatrixUniform = glGetUniformLocation(shader->glProgramName, "modelviewMatrix");
					glUniformMatrix4fv(modelviewMatrixUniform, 1, GL_FALSE, unityMatrix);

					glBindBuffer(GL_ARRAY_BUFFER, imageRenderer->quadDataBuffer);

					GLuint positionAttribute = glGetAttribLocation(shader->glProgramName, "position");
					glEnableVertexAttribArray(positionAttribute);
					glVertexAttribPointer(positionAttribute, 4 /* XYZW */, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (void*)0);

					GLint textureCoordinateAttribute = glGetAttribLocation(shader->glProgramName, "textureCoordinate");
					glEnableVertexAttribArray(textureCoordinateAttribute);
					glVertexAttribPointer(textureCoordinateAttribute, 4 /* XYZW */, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (void*)(sizeof(GLfloat)*16));

					glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (void*)0);

					glDisableVertexAttribArray(textureCoordinateAttribute);
					glDisableVertexAttribArray(positionAttribute);

					glBindVertexArray(0);
				}
				VuoShader_deactivateTextures(shader, cgl_ctx);
			}
			glUseProgram(0);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, 0, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (outputToIOSurface)
			glDeleteTextures(1, &outputTexture);

		glFlushRenderAPPLE();
	}

	if (outputToIOSurface)
		return surfID;
	else
		return outputTexture;
}

/**
 * Destroys and deallocates the image renderer.
 *
 * @threadAny
 */
void VuoImageRenderer_destroy(VuoImageRenderer ir)
{
	struct VuoImageRendererInternal *imageRenderer = (struct VuoImageRendererInternal *)ir;
	CGLContextObj cgl_ctx = (CGLContextObj)imageRenderer->glContext;

	VuoGlPool_disuse(imageRenderer->glContext, VuoGlPool_ElementArrayBuffer, sizeof(quadElements), imageRenderer->quadElementBuffer);
	VuoGlPool_disuse(imageRenderer->glContext, VuoGlPool_ArrayBuffer, sizeof(quadData), imageRenderer->quadDataBuffer);
	glDeleteVertexArrays(1, &imageRenderer->vertexArray);
	glDeleteFramebuffers(1, &imageRenderer->outputFramebuffer);
	free(imageRenderer);
}
