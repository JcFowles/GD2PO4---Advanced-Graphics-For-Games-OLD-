//=============================================================================
// Filename		: Rain.fx  
// Author		: Jc Fowles
// Discription	: Shader file used to Create a Rain Particle Effect
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
	// Constant Acceleration used to accerlate the Rain Particles Down toward the Right
	float3 g_accelerationW = { -1.0f, -9.8f, 0.0f };

	float g_lifeTime = 2.5f;
	float g_emitterRadius = 10.0f;

	// Texture coordinates used to stretch a texture over the a quad (the expanded point)
	float2 g_quadTexCoord[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
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
	float3 posW			: POSITION;
	uint type			: TYPE;
};

struct VS_FOG_OUT
{
	float3 posW			: POSITION;
	uint type			: TYPE;
	float fogLERP : FOG;
};

struct GS_OUT
{
	float4 posH			: SV_Position;
	float2 texCoord		: TEXCOORD;
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
// Procces Technique: Using a stream Outpu Only technique to Create and Destroy particles
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
		if( gIn[0].age > 0.0005f )
		{
			// Create 5 Rain Drops
			for (int i = 0; i < 5; ++i)
			{
				// Calculate a Random Vector multiplied by the emitterRadius 
				// To calculate initial position of the rain Particle
				float3 randomVector = (2*g_emitterRadius) * RandVec3((float)i/5.0f);
				// Reset the Y value
				randomVector.y = 0.0f;
					
				// Create the new Rain Particle
				Particle RainParticle;
				RainParticle.initialPosW = g_emitterPosW.xyz + randomVector;
				RainParticle.initialVelW = float3(0.0f, 0.0f, 0.0f);
				RainParticle.sizeW = float2(1.0f, 1.0f);
				RainParticle.age = 0.0f;
				RainParticle.type = PT_FLARE;


				// Add the Rain particle the Point Stream
				ptStream.Append(RainParticle);
			}
						
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
	vOut.posW = ( vIn.initialPosW) + displacement;
	
	// Store the same type
	vOut.type  = vIn.type;
	
	// Return the updated particle
	return vOut;
}


[maxvertexcount(2)]
void GS_Render(point VS_OUT gIn[1], inout LineStream<GS_OUT> lineStream)
{	
	// Dont Draw the Emitter Particle
	if (gIn[0].type != PT_EMITTER)
	{

		// Slant line in acceleration direction.
		float3 p0 = gIn[0].posW;
		float3 p1 = gIn[0].posW + 0.07f*g_accelerationW;

		float4x4 matViewProj = mul(g_matView, g_matProj);

		GS_OUT v0;
		v0.posH = mul(float4(p0, 1.0f), matViewProj);
		v0.texCoord = float2(0.0f, 0.0f);
		
		lineStream.Append(v0);

		GS_OUT v1;
		v1.posH = mul(float4(p1, 1.0f), matViewProj);
		v1.texCoord = float2(1.0f, 1.0f);
		
		lineStream.Append(v1);

	}
}

float4 PS_Render(GS_OUT pIn) : SV_TARGET
{
	// Calculate the color using the Texture array
	float arrayIndex = 0;
	float4 textureColor = g_TexureArray.Sample(gTriLinearSam, float3(pIn.texCoord, arrayIndex));

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
        
		// Set enable the depth stencil but with no depth Writes
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}