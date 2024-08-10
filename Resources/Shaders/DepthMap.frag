#version 460

void main()
{             
  /* At the end of fragment shader run the depth buffer is updated */
  gl_FragDepth = gl_FragCoord.z;
} 