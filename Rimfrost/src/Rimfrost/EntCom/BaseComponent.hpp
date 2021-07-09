#pragma once

namespace Rimfrost
{
	using ComponentTypeID = size_t;


	struct BaseComponent
	{

	};


	template<typename T>
	struct Component : public BaseComponent
	{
		/*static const componentCreateFunction create;
		static const componentRemoveFunction remove;*/
		static const ComponentTypeID s_componentID;
		static const size_t s_componentSize;
	};

}
