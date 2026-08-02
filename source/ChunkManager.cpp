#include "ChunkManager.h"
#include <algorithm>
#include <numeric>

bool ChunkManager::add(int x, int y, int z, bool rebuild) {
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end()) {
		return false;
	}
	Chunk *chunk = new Chunk(x, y, z, std::rand(), shader_info, unit_length);
	chunks[key] = chunk;
	bvh.root->createDataNode((glm::vec3(x, y, z) + glm::vec3(0.5)) * (float)CHUNK_SIZE * unit_length, chunk); // position is center of chunk to prevent floating point errors
	if(rebuild)
		bvh.rebuild();
	return true;
}

Chunk* ChunkManager::get(const glm::vec3 &pos) {
	glm::vec3 block_pos = pos / unit_length;
	int x, y, z;
	x = floor(block_pos.x / CHUNK_SIZE);
	y = floor(block_pos.y / CHUNK_SIZE);
	z = floor(block_pos.z / CHUNK_SIZE);

	return get(x, y, z);
}

Chunk* ChunkManager::get(int x, int y, int z) {
	Chunk::Key key(x, y, z);
	if (chunks.find(key) != chunks.end())
		return chunks[key];

	return nullptr;
}

bool ChunkManager::setBlock(const glm::vec3 &pos, BlockType new_type) {
	glm::vec3 block_pos = (pos / unit_length) + glm::vec3(0.5); // move to center of block to avoid floating point nonsense

	int x_ch, y_ch, z_ch;
	x_ch = floor(block_pos.x / CHUNK_SIZE);
	y_ch = floor(block_pos.y / CHUNK_SIZE);
	z_ch = floor(block_pos.z / CHUNK_SIZE);
	
	bool success = false;
	Chunk *chunk = nullptr;
	if ((chunk = get(x_ch, y_ch, z_ch))) {
		int x_b = floor(block_pos.x -= x_ch * CHUNK_SIZE);
		int y_b = floor(block_pos.y -= y_ch * CHUNK_SIZE);
		int z_b = floor(block_pos.z -= z_ch * CHUNK_SIZE);

		success = chunk->setBlock(x_b, y_b, z_b, new_type);
		std::vector<int> faces;
		if(chunk->blocks.onBoundary(&chunk->blocks(x_b, y_b, z_b), faces)) {
			for(int face : faces) {
				Chunk *n = getNeighbor(chunk, face);
				if(n) n->modified = true;
			}
		}
				
	}
	return success;
}

Chunk* ChunkManager::getNeighbor(Chunk *chunk, int face) {
	int x = chunk->x; int y = chunk->y; int z = chunk->z;
	switch (face) {
		case 0: return get(x, y + 1, z);
		case 1: return get(x, y - 1, z);
		case 2: return get(x + 1, y, z);
		case 3: return get(x - 1, y, z);
		case 4: return get(x, y, z + 1);
		case 5: return get(x, y, z - 1);
		default: return nullptr;
	}
}

Chunk::Group ChunkManager::getNeighbors(Chunk *chunk) {
	Chunk::Group neighbors(6);
	int x = chunk->x; int y = chunk->y; int z = chunk->z;

	neighbors[0] = get(x, y - 1, z);
	neighbors[1] = get(x, y + 1, z);
	neighbors[2] = get(x - 1, y, z);
	neighbors[3] = get(x + 1, y, z);
	neighbors[4] = get(x, y, z - 1);
	neighbors[5] = get(x, y, z + 1);

	return neighbors;
}

int ChunkManager::size() {
	return chunks.size();
}

ChunkManager::RaycastResult ChunkManager::raycast(const glm::vec3 &pos, const glm::vec3 &ray) {
	RaycastResult result{};
	std::vector<BVH<Chunk*>::DataNode*> hits, sorted_hits;
	bvh.raycast(pos, ray, hits);
	if(hits.size()) {
		std::vector<int> indices(hits.size());
		std::iota(indices.begin(), indices.end(), 0);
		
		std::sort(indices.begin(), indices.end(), [&](const int a, const int b) {
			return glm::length(hits[a]->pos - pos) < glm::length(hits[b]->pos - pos);
		});
		for(int i = 0; i < indices.size(); i++) {
			sorted_hits.push_back(hits[indices[i]]);
		}
		for(auto &hit : sorted_hits) {
			Chunk::RaycastResult chunk_res = hit->obj->raycast(pos, ray);
			if(chunk_res.hit) {
				result.hit = true;
				result.chunk = hit->obj;
				result.block = chunk_res.obj->block;
				result.face = chunk_res.obj->norm;
				result.pos = chunk_res.pos;
				break;
			}
		}
	}
	return result;
}

void ChunkManager::expandToFitChunk(const glm::vec3 &pos, Chunk **chunk, glm::vec3 &min, glm::vec3 &max) {
	glm::vec3 half_diameter = util::XYZ * (float)CHUNK_SIZE * (*chunk)->unit_length * 0.5f;
	min = glm::min(min, pos - half_diameter);
	max = glm::max(max, pos + half_diameter);
}