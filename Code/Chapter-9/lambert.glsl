-- vertex

#version 330 core

uniform mat4 proj;		// матрица проектирования
uniform mat4 mv;		// модельно-видовая матрица
uniform mat3 nm;		// матрица преобразования направлений
uniform vec3 lightDir;	// направления падения света

layout(location = 0) in vec3 pos;
layout(location = 2) in vec3 normal;

out vec3  n;			// выдаем вектор нормали
out vec3  l;			// и направление на источник света

void main(void)
{
			// преобразуеим точку модельно-видовой матрицей
	vec4 	p = mv * vec4 ( pos, 1.0 );
			// вычисляем итовые координаты
	gl_Position  = proj * p;
			// нормаль преобразуется при помощи специальной матрицы
	n            = normalize ( nm * normal );
			// считаем, что у нас бесконечно-удаленный источник света
	l            = normalize ( lightDir );
}

-- fragment

#version 330 core

in	vec3 n;
in	vec3 l;

out vec4 color;

void main(void)
{
	vec3  n2   = normalize ( n );	// нормируем интерполированную нормаль
	vec3  l2   = normalize ( l );	// нормируем вектор на источник света
	float diff = max ( dot ( n2, l2 ), 0.0 );
	vec4  clr  = vec4 ( 0.7, 0.1, 0.1, 1.0 );
	float ka   = 0.2;
	float kd   = 0.8;

	color = (ka + kd*diff) * clr;
}
