#include "bdc_cage_test/pch.h"

#include "gtest/gtest.h"

#include "riaecs/include/asset.h"
#pragma comment(lib, "riaecs.lib")

#include "bdc_cage/include/system_cage.h"
#include "bdc_cage/include/component_cage.h"
#pragma comment(lib, "bdc_cage.lib")

#include "mono_transform/include/component_transform.h"
#pragma comment(lib, "mono_transform.lib")

#include "mono_delta_time/include/delta_time_provider.h"
#pragma comment(lib, "mono_delta_time.lib")

TEST()
{
	mono_transform::ComponentTransform::SetupParam setup;
	mono_transform::ComponentTransform component;
	component.Setup(setup);

	bdc_cage::ComponentCage::SetupParam st;
	st.moveSpeed_ = 3.0f;
	st.targetPos_ = DirectX::XMFLOAT3(-30.5f, 0, 30.5f);
	st.cageSize_ = 35;
	st.roamCount_ = 5;
	st.numDivisions_ = 20;
	bdc_cage::ComponentCage cage;

	cage.Setup(st);

	mono_delta_time::DeltaTimeProvider deltaTimeProvider_;
	bdc_cage::GridDirection dir = bdc_cage::GridDirection::East;

	while (true)
	{
		deltaTimeProvider_.UpdateTime();
		DirectX::XMFLOAT3 pos = bdc_cage::updateCagePos(&cage,&component, dir, 96,deltaTimeProvider_.GetDeltaTime());
		mono_transform::ComponentTransform::SetupParam setup;
		setup.pos_ = pos;
		component.Setup(setup);

		std::cout << "transform: x=" << pos.x
			<< ", y=" << pos.y
			<< ", z=" << pos.z << std::endl;
	}
}