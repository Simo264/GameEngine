#pragma once

#include "Core/Core.hpp"

/**
 * Render primitives from array data
 * 
 * @param mode: specifies what kind of primitives to render. Symbolic constants 
 *							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, 
 *							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, 
 *							GL_TRIANGLES_ADJACENCY and GL_PATCHES
 */
void DrawArrays(uint32_t mode, class VertexArray& vertexArray);

/**
 * Draw multiple instances of a range of elements.
 * 
 * @param mode: specifies what kind of primitives to render. Symbolic constants 
 *							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, 
 *							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, 
 *							GL_TRIANGLES_ADJACENCY and GL_PATCHES
 * 
 * @param nInstances: specifies the number of instances of the specified range of indices to be rendered.
 */
void DrawArraysInstanced(uint32_t mode, class VertexArray& vertexArray, int nInstances);


/**
 * Render primitives from array data.
 * 
 * @param mode: specifies what kind of primitives to render. Symbolic constants 
 *							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, 
 *							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, 
 *							GL_TRIANGLES_ADJACENCY and GL_PATCHES
 */
void DrawElements(uint32_t mode, class VertexArray& vertexArray);

/**
 * Draw multiple instances of a set of elements.
 * 
 * @param mode: specifies what kind of primitives to render. Symbolic constants 
 *							GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY, GL_LINES_ADJACENCY, 
 *							GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_TRIANGLE_STRIP_ADJACENCY, 
 *							GL_TRIANGLES_ADJACENCY and GL_PATCHES
 * 
 * @param nInstances: specifies the number of instances of the specified range of indices to be rendered.
 */
void DrawElementsInstanced(uint32_t mode, class VertexArray& vertexArray, int nInstances);
