/*
* Bachelor of Software Engineering
* Media Design School
* Auckland
* New Zealand
*
* (c) 2005 - 2015 Media Design School
*
* File Name : color.fx 
* Description : Shader File to calculate transfroms and Color
* Author :	Jc Fowles
* Mail :	Jc.Fowles@mediadesign.school.nz
*/

/***********************
* cbPerObject: Constant Buffer for technique Variables
* @author: Jc Fowles
* @return:
********************/
cbuffer cbPerObject
{
	float4x4 matWorld;
	float4x4 matView;  
	float4x4 matProj; 	

	float4 objColor;
	float time;
};

/***********************
* VertexShaderInput: Struct to hold the vertex shader input
* @author: Jc Fowles
* @return:
********************/
struct VertexShaderInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

/***********************
* PixelShaderInput: Struct to hold the Pixel shader input
* @author: Jc Fowles
* @return:
********************/
struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

/***********************
* ColorVertexShader: Calculate the transfroms into world view projection of each vertex
* @author: Jc Fowles
* @Parameter: inputVS: Vertices to vertex transformation on 
* @return: PixelShaderInput: The transformed Vertices to be send to the pixel Shader
********************/
PixelShaderInput ColorVertexShader(VertexShaderInput inputVS)
{
	PixelShaderInput outputPS;
	
	// homogeneous the postion vector
	inputVS.position.w = 1.0f;

	// Transform the postion based on World, View and Projection
	outputPS.position = mul(inputVS.position, matWorld);
	outputPS.position = mul(outputPS.position, matView);
	outputPS.position = mul(outputPS.position, matProj);

	// Store the input color for the pixel shader to use.
	outputPS.color = objColor; //inputVS.color;

	return outputPS;
}

/***********************
* TimeVertexShader: Calculate the transfroms into world view projection of each vertex and color as a function of time
* @author: Jc Fowles
* @Parameter: inputVS: Vertices to vertex transformation on
* @return: PixelShaderInput: The transformed Vertices to be send to the pixel Shader
********************/
PixelShaderInput TimeVertexShader(VertexShaderInput inputVS)
{
	PixelShaderInput outputPS;

	// homogeneous the postion vector
	inputVS.position.w = 1.0f;

	// Transform the postion based on World, View and Projection
	outputPS.position = mul(inputVS.position, matWorld);
	outputPS.position = mul(outputPS.position, matView);
	outputPS.position = mul(outputPS.position, matProj);

	
	// Change the vertex color based on the y position and time
	if ((inputVS.position.y > 0) && (inputVS.position.y <= 4))
	{
		outputPS.color.x = time;
		outputPS.color.y = objColor.y;
		outputPS.color.z = objColor.z;
		outputPS.color.w = objColor.w;
	}
	else if ((inputVS.position.y > 4) && (inputVS.position.y <= 8))
	{
		outputPS.color.x = objColor.x;
		outputPS.color.y = time;
		outputPS.color.z = objColor.z;
		outputPS.color.w = objColor.w;
	}
	else
	{
		outputPS.color.x = objColor.x;
		outputPS.color.y = objColor.y;
		outputPS.color.z = time;
		outputPS.color.w = objColor.w;
	}
		
	return outputPS;
}

/***********************
* ColorPixelShader: Calculate the color of each pixel between the Vertices
* @author: Jc Fowles
* @Parameter: inputPS: Vertices to calculate color from
* @return: float4: Color value of the pixel 
********************/
float4 ColorPixelShader(PixelShaderInput inputPS) : SV_Target
{
	return inputPS.color;
}

/***********************
* ColorTechnique: TO DO:
* @author: Jc Fowles
* @return: 
********************/
technique10 ColorTechnique
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, ColorVertexShader()));
		SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
		SetGeometryShader(NULL);
	}
}

/***********************
* ColorChangeTechnique: TO DO:
* @author: Jc Fowles
* @return:
********************/
technique10 ColorChangeTechnique
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, TimeVertexShader()));
		SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
		SetGeometryShader(NULL);
	}
}