#include "pch.h"
#include "Cell.h"

const float Cell::s_size = 25;
int Cell::s_instances = 0;

Cell::Cell(std::string texture_name) :
	m_id(s_instances++),
	m_texture_name(texture_name)
{
}

Cell::~Cell()
{
}

void Cell::draw()
{	
	Renderer2D::pushQuad(m_transform.get_world() * glm::scale(glm::mat4(1.0f), {s_size, s_size, 1}), TextureManager::get(m_texture_name), glm::vec4(1.0f), false);
}
