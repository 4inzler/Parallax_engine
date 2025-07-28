//// GridPass.hpp /////////////////////////////////////////////////////////////
//
//  zzzzz      zzz   zzzzzzzzzzzzz   zzzz      zzzz      zzzzz  zzzzz
//  zzz zzz    zzz   zzzz              zzzz  zzzz      zzzz        zzzz
//  zzz   zzz  zzz   zzzzzzzzz            zzzz        zzzz          zzzz
//  zzz    zzz zzz   zzzz              zzzz  zzzz      zzzz        zzzz
//  zzz      zzzzz   zzzzzzzzzzzzz   zzzz      zzzz      zzzzz  zzzzz
//
//  Author:      Mehdy MORVAN
//  Date:        09/05/2025
//  Description: Header file for the grid render pass
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "renderer/RenderPass.hpp"
#include "renderer/Framebuffer.hpp"

namespace nexo::renderer {

    class GridPass : public RenderPass {
        public:
            GridPass();
            ~GridPass() override = default;

            void execute(RenderPipeline& pipeline) override;
    };
}
