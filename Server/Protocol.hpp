#pragma once

namespace Protocol
{
	enum Protocol : short
	{
		Message, //Chat messages
		ConnectDetails, //Received client details after a successful connection
		PlayerConnect, //When a player connects
		PlayerUpdate, //Player data update event
		PlayerDisconnect, //When a player disconnects
		ClientDisconnect, //Forces the client to disconnect
		RequestChunk, //Client requests a chunk from server
		ChunkResponse, //Client receives the requested chunk from the server
		ChunkUpdate
	};

	enum Biome : short
	{
		Field,
		Forest,
		RainForest,
		Tundra,
		Desert,
		Rocky,
	};

	enum Entity : short
	{
		Tree,
		Grass,
		Lake,
		HouseWall
	};

	enum TileTexture : short
	{
		TextureGrass,
		TextureForestGrass,
		TextureRocky,
		TextureSnowy,
		TextureDesert
	};

	enum TileFootstep : short
	{
		FootstepGrass,
		FootstepGravel,
		FootstepDirt
	};


	extern const int version;
}
