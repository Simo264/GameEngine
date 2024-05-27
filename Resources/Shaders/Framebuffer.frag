#version 460

in vec2 TextCoords;
out vec4 FragColor;

uniform sampler2D u_screenTexture;

/*
	UPostProcessingType=0 -> POST_PROC_NONE			
	UPostProcessingType=1 -> POST_PROC_INVERSION 
	UPostProcessingType=2 -> POST_PROC_GRAYSCALE 
	UPostProcessingType=3 -> POST_PROC_KERNEL		
	UPostProcessingType=4 -> POST_PROC_BLUR			
*/
//uniform int UPostProcessingType;

/* ----------------------------
			Post Processing
---------------------------- */
//vec4 InversionEffect();
//vec4 GrayscaleEffect();
//vec4 KernelEffect();
//vec4 BlurEffect();


/* ----------------------------
			Global variables 
---------------------------- */
vec3 texColor;

void main()
{
	texColor = texture(u_screenTexture, TextCoords).rgb;
	FragColor = vec4(texColor, 1.0);

	//	switch(UPostProcessingType)
	//	{
	//	case 0:
	//		FragColor = vec4(texColor, 1.0);
	//		break;
	//	case 1: 
	//		FragColor = InversionEffect();
	//		break;
	//	case 2: 
	//		FragColor = GrayscaleEffect();
	//		break;
	//	case 3: 
	//		FragColor = KernelEffect();
	//		break;
	//	case 4: 
	//		FragColor = BlurEffect();	
	//		break;
	//	default: 
	//		FragColor = vec4(texColor, 1.0);
	//		break;
	//	}
}


//vec4 InversionEffect()
//{
//	return vec4(vec3(1.0 - texColor), 1.0);
//}
//
//vec4 GrayscaleEffect()
//{
//	float average = 0.2126 * texColor.r + 0.7152 * texColor.g + 0.0722 * texColor.b;
//	return vec4(average, average, average, 1.0);
//}
//
//vec4 KernelEffect()
//{
//	const float offset = 1.0 / 300.0;  
//	vec2 offsets[9] = vec2[](
//		vec2(-offset,  offset), /* top-left */
//		vec2( 0.0f,    offset), /* top-center */
//		vec2( offset,  offset), /* top-right */
//		vec2(-offset,  0.0f),   /* center-left */
//		vec2( 0.0f,    0.0f),   /* center-center */
//		vec2( offset,  0.0f),   /* center-right */
//		vec2(-offset, -offset), /* bottom-left */
//		vec2( 0.0f,   -offset), /* bottom-center */
//		vec2( offset, -offset)  /* bottom-right */
//	);
//	float kernel[9] = float[](
//		-1, -1, -1,
//		-1,  9, -1,
//		-1, -1, -1
//	);
//	vec3 sampleTex[9];
//	for(int i = 0; i < 9; i++)
//		sampleTex[i] = vec3(texture(UScreenTexture, TextCoords.st + offsets[i]));
//	
//	vec3 col = vec3(0.0);
//	for(int i = 0; i < 9; i++)
//		col += sampleTex[i] * kernel[i];
//
//	return vec4(col, 1.0);
//}
//
//vec4 BlurEffect()
//{
//	const float offset = 1.0 / 300.0;  
//	vec2 offsets[9] = vec2[](
//		vec2(-offset,  offset), /* top-left */
//		vec2( 0.0f,    offset), /* top-center */
//		vec2( offset,  offset), /* top-right */
//		vec2(-offset,  0.0f),   /* center-left */
//		vec2( 0.0f,    0.0f),   /* center-center */
//		vec2( offset,  0.0f),   /* center-right */
//		vec2(-offset, -offset), /* bottom-left */
//		vec2( 0.0f,   -offset), /* bottom-center */
//		vec2( offset, -offset)  /* bottom-right */
//	);
//	float kernel[9] = float[](
//    1.0/16, 2.0/16, 1.0/16,
//    2.0/16, 4.0/16, 2.0/16,
//    1.0/16, 2.0/16, 1.0/16  
//	);
//
//	vec3 sampleTex[9];
//	for(int i = 0; i < 9; i++)
//		sampleTex[i] = vec3(texture(UScreenTexture, TextCoords.st + offsets[i]));
//	
//	vec3 col = vec3(0.0);
//	for(int i = 0; i < 9; i++)
//		col += sampleTex[i] * kernel[i];
//
//	return vec4(col, 1.0);
//
//}