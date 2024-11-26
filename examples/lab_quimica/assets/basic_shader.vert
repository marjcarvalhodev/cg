#version 300 es

layout(location = 0) in vec3 inPosition;

void main() {
  float sinAngle = sin(0.5);
  float cosAngle = cos(0.5);

  gl_Position =
      vec4(inPosition.x * cosAngle + inPosition.z * sinAngle, inPosition.y,
           inPosition.z * cosAngle - inPosition.x * sinAngle, 1.0);
}