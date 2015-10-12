//=============================================================================
// Filename		: FallingStars.fx  
// Author		: Jc Fowles
// Discription	: Shader file used to Create a Falling Stars Particle Effect
//=============================================================================

// Local Includes

//=============================================================
// Global Shader Variables                                      
//=============================================================

cbuffer cbPerFrame
{
	float4 g_cameraPosW;
	float4x4 g_matView;
	float4x4 g_matProj;

	float g_gameTime;
	float g_timeStep;

	// Particle Variables
	float4 g_emitterPosW;
	float4 g_emitterDirW;

};

cbuffer cbFixed
{
	// Constant Acceleration used to accerlate the Falling Star Particles Down toward the Right
	float3 g_accelerationW = { -1.0f, -7.8f, 0.0f };

	float g_lifeTime = 5.5f;
	float g_emitterRadius = 5.0f;

};
 
// Nonnumeric values cannot be added to a cbuffer
Texture2DArray g_TexureArray;

// Randomly Created 1D Texture used to Generate Random Numbers within this shader
Texture1D g_RandomTexure;
 
//=============================================================
// Shader States
//=============================================================

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//=============================================================
// Shader Structs
//=============================================================

#define PT_EMITTER	0
#define PT_FLARE	1

struct Particle
{
	float3 initialPosW	: POSITION;
	float3 initialVelW	: VELOCITY;
	float2 sizeW		: SIZE;
	float age			: AGE;
	uint type			: TYPE;
};

struct VS_OUT
{
	float3 posW		: POSITION;
	uint type		: TYPE;
	float2 sizeW	: SIZE;

};

struct GS_OUT
{
	float4 posH		: SV_Position;
};

//=============================================================
// Utility Functions
//=============================================================

float3 RandUnitVec3(float _offset)
{
	// Sample the Random 1D Texture using the Game Time and Adding an Offset 
	float TexCoord = (g_gameTime + _offset);
	float3 randomVector = (float3)g_RandomTexure.SampleLevel(gTriLinearSam, TexCoord, 0);

		// Normalise and Return
		return normalize(randomVector);
}

float3 RandVec3(float _offset)
{
	// Sample the Random 1D Texture using the Game Time and Adding an Offset 
	float TexCoord = (g_gameTime + _offset);
	float3 randomVector = (float3)g_RandomTexure.SampleLevel(gTriLinearSam, TexCoord, 0);

	return randomVector;
}

//=============================================================
// Procces Technique: Using a stream Output Only technique to Create and Destroy particles
//=============================================================
  
Particle VS_Process(Particle vIn)
{
	// Just return the Particle unchanged
	return vIn;
}

[maxvertexcount(6)]
void GS_Process(point Particle gIn[1], inout PointStream<Particle> ptStream)
{	
	// Add the time step to the age of the particle
	gIn[0].age += g_timeStep;
	
	// Check if this particle is an Emitter
	if( gIn[0].type == PT_EMITTER )
	{	
		// Is it time to Emit the next Paricle
		if( gIn[0].age > 0.5f )
		{
			
			// Calculate a Random Vector multiplied by the emitterRadius 
			// To calculate initial position of the Star Particle
			float3 randomVector = (2 * g_emitterRadius) * RandVec3(0.0f);
			// Reset the Y value
			randomVector.y = 0.0f;
			
			// Create the new Star Particle
			Particle StarParticle;
			StarParticle.initialPosW = g_emitterPosW.xyz + randomVector;
			StarParticle.initialVelW = float3(0.0f, 0.0f, 0.0f);
			StarParticle.sizeW = float2(3.0f, 3.0f);
			StarParticle.age = 0.0f;
			StarParticle.type = PT_FLARE;

			// Add the Star particle the Point Stream
			ptStream.Append(StarParticle);
									
			// Then Reset the Emitters Age
			gIn[0].age = 0.0f;
		}
		
		// Add the emitter the Point Stream
		ptStream.Append(gIn[0]);
	}
	else
	{
		// If the Paricle is not the Emitter check its age
		// if its Older than One Second Destroy it (Dont Add it to the stream)
		if (gIn[0].age <= g_lifeTime)
		{
			// Add all Alive particles to the Point Stream
			ptStream.Append(gIn[0]);
		}
	}		
}

// Contruct the Geometry Shader
GeometryShader gsProcess = ConstructGSWithSO(CompileShader(gs_4_0, GS_Process()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique10 StreamOutTech
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_4_0, VS_Process()));
		SetGeometryShader(gsProcess);
        
		// Disable Rendering and Rasterization
		// Disable the Pixel shader as we are not ready to Draw
        SetPixelShader(NULL);
  
		// The Depth buffer will also need to be disabled during the stream-out process
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//=============================================================
// Render Technigue : Used to draw the Particles
//=============================================================

VS_OUT VS_Render(Particle vIn)
{
	VS_OUT vOut;
	
	float time = vIn.age;
	
	// Calculate the particles position using the Constant Acceleration Equaltion 
	// s = v(i)*t + 1/2*a*t^2
	float3 displacement = (time * vIn.initialVelW) + ((1.0f / 2.0f)*(time*time)*g_accelerationW);
	vOut.posW = vIn.initialPosW + displacement;

	// Store the same type
	vOut.type  = vIn.type;

	vOut.sizeW = vIn.sizeW;
	
	// Return the updated particle
	return vOut;
}



[maxvertexcount(6)]
void GS_Render(point VS_OUT gIn[1], inout TriangleStream<GS_OUT> triStream)
{
	// Dont Draw the Emitter Particle
	if (gIn[0].type != PT_EMITTER)
	{

		// Calculate a world matrix so that the Star Particle "Billboard" always faces the camera
		float3 look = normalize(g_cameraPosW.xyz - gIn[0].posW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		float4x4 matWorld;
		matWorld[0] = float4(right, 0.0f);
		matWorld[1] = float4(up, 0.0f);
		matWorld[2] = float4(look, 0.0f);
		matWorld[3] = float4(gIn[0].posW, 1.0f);

		// Calculate the World View Projection Matrix
		float4x4 matViewProj = mul(g_matView, g_matProj);
		float4x4 WVP = mul(matWorld, matViewProj);

		// Expand the particle point into two triangles to create the Billboarded Star
		float4 particleStar[6];

		// Triangle One
		particleStar[0] = float4(gIn[0].posW.x + 0.0f, gIn[0].posW.y + 1.732f, 1.0f, 1.0f);
		particleStar[1] = float4(gIn[0].posW.x - 1.5f, gIn[0].posW.y - 0.866f, 1.0f, 1.0f);
		particleStar[2] = float4(gIn[0].posW.x + 1.5f, gIn[0].posW.y - 0.866f, 1.0f, 1.0f);
				
		// Triangle Two
		particleStar[3] = float4(gIn[0].posW.x + 1.5f, gIn[0].posW.y + 0.866f, 1.0f, 1.0f);
		particleStar[4] = float4(gIn[0].posW.x - 1.5f, gIn[0].posW.y + 0.866f, 1.0f, 1.0f);
		particleStar[5] = float4(gIn[0].posW.x - 0.0f, gIn[0].posW.y - 1.732f, 1.0f, 1.0f);

		GS_OUT gOut;

		// Transform the Star into world space and add it to the Triangle Stream
		[unroll]
		for (int i = 0; i < 6; i++)
		{
			// Muliply the new point with the World View Projection matrix
			gOut.posH = mul(particleStar[i], WVP);
			triStream.Append(gOut);

			// Restart the stream to start the next triangle
			if (i == 2)
			{
				triStream.RestartStrip();
			}
		}
		// Restart the stream
		triStream.RestartStrip();

	}
}

float4 PS_Render(GS_OUT pIn) : SV_TARGET
{
	// Calculate a random color
	float4 textureColor = float4(RandVec3(g_timeStep),1.0f);

	// Return the texture color
	return textureColor;
}

technique10 RenderTech
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_4_0, VS_Render()));
		SetGeometryShader(CompileShader(gs_4_0, GS_Render()));
		SetPixelShader(CompileShader(ps_4_0, PS_Render()));
        
		// Set Blend State for additive blending 
       SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		// Also set enable the depth stencil but with no depth Writes
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}