#version 300 es

precision mediump float;

out vec4 outColor;

void main() {
  float i = 1.0 - gl_FragCoord.z;

  outColor = vec4(i);
}