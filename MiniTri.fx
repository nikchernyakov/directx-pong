cbuffer cbGlobal : register(b0)
{
	matrix MOVE;
}

struct VS_DATA
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

VS_DATA VSMain(VS_DATA input )
{
	VS_DATA output = (VS_DATA)0;
	
	output.pos = mul(input.pos, MOVE);
	//output.pos = input.pos;
	output.col = input.col;
	
	return output;
}

float4 PSMain( PS_IN input ) : SV_Target
{
	float4 col = input.col;
#ifdef TEST
	if (input.pos.x > 400) col = TCOLOR;
#endif

	return col;
}
