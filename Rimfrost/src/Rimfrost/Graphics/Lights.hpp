#pragma once


#include "Rimfrost\Graphics\PointLightContainer.hpp"



namespace Rimfrost
{
	struct Lights
	{
		std::shared_ptr<PointLightContainer> pointLights = nullptr;
	};
}

