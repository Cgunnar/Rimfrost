#pragma once

#include "Rimfrost\EntCom\rfEntity.hpp"
#include "Rimfrost\Math\RimfrostMath.hpp"
#include "Rimfrost\Scene\Node.hpp"

namespace Rimfrost
{
	struct NodeComponent : Rimfrost::Component<NodeComponent>
	{
		NodeHandle nodeHandel;
	};
}