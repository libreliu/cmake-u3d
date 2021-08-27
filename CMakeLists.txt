project(U3D)
cmake_minimum_required(VERSION 3.10)

# Configurations related to RTL/Platform
if(MSVC)
    set(U3D_PLATFORM "Win32")
else()
    message(FATAL_ERROR "Platform not supported")
endif()

# Routines
# Ubpa saikou!
function(Ubpa_List_Print)
  cmake_parse_arguments("ARG" "" "TITLE;PREFIX" "STRS" ${ARGN})
  list(LENGTH ARG_STRS strsLength)
  if(NOT strsLength)
    return()
  endif()
  if(NOT ${ARG_TITLE} STREQUAL "")
    message(STATUS ${ARG_TITLE})
  endif()
  foreach(str ${ARG_STRS})
    message(STATUS "${ARG_PREFIX}${str}")
  endforeach()
endfunction()

function(GlobHdrRecurse ret targetPath)
  file(GLOB_RECURSE hdrs "${targetPath}/*.h" "${targetPath}/*.hpp")
  set (${ret} "${hdrs}" PARENT_SCOPE)
endfunction()

function(GlobSrcRecurse ret targetPath)
  file(GLOB_RECURSE hdrs "${targetPath}/*.cpp" "${targetPath}/*.c")
  set (${ret} "${hdrs}" PARENT_SCOPE)
endfunction()

# Dependencies - zlib
GlobHdrRecurse(ZLIB_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/zlib)
GlobSrcRecurse(ZLIB_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/zlib)
Ubpa_List_Print(TITLE "Zlib Headers:" STRS ${ZLIB_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "Zlib Sources:" STRS ${ZLIB_SRCS} PREFIX "  ")

add_library(zlib STATIC ${ZLIB_HDRS} ${ZLIB_SRCS})
target_include_directories(zlib PUBLIC ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/zlib/Source)

configure_file(${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/zlib/Source/zconf.h.cmakein
               ${CMAKE_CURRENT_BINARY_DIR}/zlib/zconf.h @ONLY)

target_include_directories(zlib PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/zlib/)

# Dependencies - png
GlobHdrRecurse(PNG_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/png)
GlobSrcRecurse(PNG_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/png)
Ubpa_List_Print(TITLE "PNG Headers:" STRS ${PNG_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "PNG Sources:" STRS ${PNG_SRCS} PREFIX "  ")

add_library(libpng STATIC ${PNG_HDRS} ${PNG_SRCS})
target_include_directories(libpng PUBLIC ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/png/Source)
target_link_libraries(libpng zlib)

# Dependencies - jpeg
GlobHdrRecurse(JPEG_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/jpeg)
GlobSrcRecurse(JPEG_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/jpeg)
Ubpa_List_Print(TITLE "JPEG Headers:" STRS ${JPEG_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "JPEG Sources:" STRS ${JPEG_SRCS} PREFIX "  ")

add_library(libjpeg STATIC ${JPEG_HDRS} ${JPEG_SRCS})
target_include_directories(libjpeg PUBLIC ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/jpeg)

configure_file(${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/jpeg/jconfig.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/jpeg/jconfig.h)
target_include_directories(libjpeg PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/jpeg)

target_link_libraries(libjpeg zlib libpng)

# Dependencies - wcmatch
GlobHdrRecurse(WCMATCH_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards)
GlobSrcRecurse(WCMATCH_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards)
Ubpa_List_Print(TITLE "WCMatch Headers:" STRS ${WCMATCH_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "WCMatch Sources:" STRS ${WCMATCH_SRCS} PREFIX "  ")

add_library(wcmatch STATIC ${WCMATCH_HDRS} ${WCMATCH_SRCS})
target_include_directories(wcmatch PUBLIC ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards)

# Dependencies - predicate
GlobHdrRecurse(PREDICATE_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/Predicates)
GlobSrcRecurse(PREDICATE_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/Predicates)
Ubpa_List_Print(TITLE "Predicates Headers:" STRS ${PREDICATE_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "Predicates Sources:" STRS ${PREDICATE_SRCS} PREFIX "  ")

add_library(predicates INTERFACE)
target_sources(predicates INTERFACE ${PREDICATE_HDRS} ${PREDICATE_SRCS})
target_include_directories(predicates INTERFACE ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/Predicates)

# Dependencies - FNVHash
add_library(FNVHash INTERFACE)
target_include_directories(FNVHash INTERFACE ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/FNVHash)

# Dependencies - dxsdk
add_library(dxsdk INTERFACE)
target_include_directories(dxsdk INTERFACE ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/dxsdk/Include)

# Common Infrastructure
GlobHdrRecurse(Component_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Include)
GlobHdrRecurse(Kernel_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Include)
GlobHdrRecurse(Platform_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Include)
Ubpa_List_Print(TITLE "Component Headers:" STRS ${Component_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "Kernel Headers:" STRS ${Kernel_HDRS} PREFIX "  ")
Ubpa_List_Print(TITLE "Platform Headers:" STRS ${Platform_HDRS} PREFIX "  ")

set(IFX_INCLUDES
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards
    # IFXOSRenderWindow needs this
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering
)

add_library(IFXCommon INTERFACE)
target_compile_definitions(IFXCommon INTERFACE UNICODE _CRT_SECURE_NO_WARNINGS U3D_NO_ASM $<$<CONFIG:DEBUG>:DEBUG>)
target_link_libraries(IFXCommon INTERFACE libpng libjpeg zlib FNVHash wcmatch winmm)

# IFXCoreStatic
# This provides C functions like IFXCreateComponent
# which is redirected into call to IFXCore Dynamic Libaray

set(IFXCoreStatic_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)

set(IFXCoreStatic_SRCS
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCoreStatic/IFXCoreStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/IFXDids.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXCoincidentVertexMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXCornerIter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXEuler.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXFatCornerIter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXTransform.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVectorHasher.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVertexMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVertexMapGroup.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards/wcmatch.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
)

add_library(IFXCoreStatic STATIC ${IFXCoreStatic_SRCS} ${IFXCoreStatic_HDRS})

target_include_directories(IFXCoreStatic PUBLIC ${IFX_INCLUDES})
target_link_libraries(IFXCoreStatic PUBLIC IFXCommon)

# IFXCore Dynamic Library

set(IFXCore_SRCS
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/IFXCore/IFXCoreDllMain.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXCoincidentVertexMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXCornerIter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXEuler.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXFatCornerIter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXTransform.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVectorHasher.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVertexMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVertexMapGroup.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BitStream/CIFXBitStreamX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BitStream/CIFXDataBlockQueueX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BitStream/CIFXDataBlockX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BitStream/IFXHistogramDynamic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/CIFXAnimationModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/CIFXBoneWeightsModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/CIFXSkeleton.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXBoneCacheArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXBoneNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXBonesManagerImpl.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXCharacter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXCoreNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXKeyTrack.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMeshGroup_Character.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMixerQueue.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMixerQueueImpl.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMotion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMotionManagerImpl.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXMotionMixerImpl.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXSkin.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXSkin_p3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXVertexWeight.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones/IFXVertexWeights.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXAABBHierarchyBuilder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXAxisAlignedBBox.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXBoundFace.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXBoundHierarchy.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXBoundUtil.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXBTree.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXBTreeNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXCollisionResult.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXPickObject.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXPrimitiveOverlap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy/CIFXResultAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/CIFXAuthorCLODGen.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/CIFXAuthorMeshMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/CIFXSetAdjacencyX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/CIFXSetX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/CLODGenerator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/ContractionRecorder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/Face.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/Matrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/NormalMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/Pair.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/PairFinder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/PairHash.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/PairHeap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/Primitives.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/Vertex.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor/VertexPairContractor.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXCoreServices.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXCoreServicesRef.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXHashMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXIDManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXMetaData.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXNameMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXSimpleHash.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXVoidWrapper.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/IFXComponentDescriptorList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/IFXComponentIds.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/IFXDids.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXAuthorCLODResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXAuthorMesh.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXAuthorMeshScrub.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXCLODModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXMeshCompiler.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/CIFXMeshMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/IFXCLODManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD/IFXNeighborResController.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXContour.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXContourExtruder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXContourGenerator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXContourTessellator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXGlyph2DCommands.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXGlyph2DModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXGlyph3DGenerator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXGlyphCommandList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D/CIFXQuadEdge.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/LineSet/CIFXAuthorLineSetAnalyzer.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/LineSet/CIFXAuthorLineSet.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/LineSet/CIFXAuthorLineSetResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/PointSet/CIFXAuthorPointSet.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/PointSet/CIFXAuthorPointSetResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/CIFXInterleavedData.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/CIFXMesh.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/CIFXMeshGroup.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/CIFXNeighborMesh.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/CIFXRenderable.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh/IFXFaceLists.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXDidRegistry.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXModifierChain.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXModifierDataElementIter.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXModifierDataPacket.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXSubject.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/IFXModifierChainState.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Palette/CIFXPalette.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Palette/CIFXSimpleObject.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXBoundSphereDataElement.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXDevice.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXDummyModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXFileReference.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXGroup.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXLight.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXLightResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXLightSet.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXMarker.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXMaterialResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXMixerConstruct.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXModel.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXMotionResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXResourceClient.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXSceneGraph.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXShaderList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXSimpleCollection.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXSimpleList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXView.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXViewResource.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/IFXRenderPass.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Shaders/CIFXShader.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Shaders/CIFXShaderLitTexture.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Shaders/CIFXShadingModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/CIFXSubdivModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXAttributeNeighborhood.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXButterflyScheme.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXScreenSpaceMetric.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXSharedUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXSubdivisionManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXTQTAddress.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXTQTAttribute.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXTQTBaseTriangle.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXTQTTriangleAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXTQTTriangle.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv/IFXVertexAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Texture/CIFXImageTools.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Texture/CIFXTextureImageTools.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Texture/CIFXTextureObject.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVGenerator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVMapperCylindrical.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVMapperNone.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVMapperPlanar.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVMapperReflection.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator/CIFXUVMapperSpherical.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/CIFXConnector.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/CIFXPerformanceTimer.cpp 
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXCheckX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/IFXCom/CIFXComponentManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/IFXCom/CIFXGUIDHashMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/IFXCom/CIFXPluginProxy.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/IFXCom/IFXCom.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Memory/IFXMemory.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards/wcmatch.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/Predicates/predicates.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
    ${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXIPP.cpp
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/Common/IFXOSRender.cpp
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/Common/IFXOSFileIterator.cpp
)

set(IFXCore_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)

add_library(IFXCore SHARED
    ${IFXCore_SRCS}
    ${IFXCore_HDRS}
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXCore/IFXCore.rc
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXCore/IFXResource.h
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXCore/IFXCore.def
)

target_include_directories(IFXCore PUBLIC 
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Include
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BitStream
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Bones
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/BoundHierarchy
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/CLODAuthor
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/CLOD
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/Glyph2D
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/LineSet
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Generators/PointSet
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Mesh
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Palette
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Shaders
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Subdiv
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Texture
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/UVGenerator
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/IFXCom
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/FNVHash
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/Predicates
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards
)

target_link_libraries(IFXCore zlib libpng libjpeg FNVHash predicates IFXCommon)

# IFXExporting
set(IFXExporting_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)
set(IFXExporting_SRCS
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/IFXExporting/IFXExportingDllMain.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXAnimationModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXAuthorCLODEncoderX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXAuthorCLODEncoderX_P.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXAuthorCLODEncoderX_S.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXAuthorGeomCompiler.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXBlockPriorityQueueX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXBlockWriterX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXBoneWeightsModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXCLODModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXDummyModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXFileReferenceEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXGlyphModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXGroupNodeEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXLightNodeEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXLightResourceEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXLineSetEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXMaterialResourceEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXModelNodeEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXMotionResourceEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXNodeBaseEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXPointSetEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXShaderLitTextureEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXShadingModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXStdioWriteBufferX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXSubdivisionModifierEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXViewNodeEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXViewResourceEncoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/CIFXWriteManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/IFXExporting.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Exporting/IFXExportingGuids.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Base/IFXVertexMap.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards/wcmatch.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
)

add_library(IFXExporting SHARED
    ${IFXExporting_SRCS}
    ${IFXExporting_HDRS}
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXExporting/IFXExporting.rc
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXExporting/IFXResource.h
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXExporting/IFXExporting.def
)

target_link_libraries(IFXExporting IFXCore IFXCommon)

set(IFXImporting_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)

set(IFXImporting_SRCS
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/IFXImporting/IFXImportingDllMain.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXAnimationModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXAuthorCLODDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXAuthorCLODDecoder_P.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXAuthorCLODDecoder_S.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXBlockReaderX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXBoneWeightsModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXBTTHash.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXCLODModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXDecoderChainX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXDummyModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXGlyphModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXGroupDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXInternetReadBufferX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXLightDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXLightResourceDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXLineSetDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXLoadManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXMaterialDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXModelDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXMotionDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXNodeDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXPointSetDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXShaderLitTextureDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXShadingModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXStdioReadBufferX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXSubdivisionModifierDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXTextureDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXViewDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/CIFXViewResourceDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXImporting.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXImportingGuids.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXInternetConnectionX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXInternetSessionX.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXModifierBaseDecoder.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Importing/IFXSocketStream.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSLoader.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSSocket.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards/wcmatch.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
)

add_library(IFXImporting SHARED
	${IFXImporting_SRCS}
	${IFXIMPORTING_HDRS}
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXImporting/IFXImporting.rc
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXImporting/IFXResource.h
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXImporting/IFXImporting.def
)
# Require winsock2 to make IFXOSSocket.cpp work
target_link_libraries(IFXImporting IFXCore IFXCommon ws2_32)

# IFXScheduling
set(IFXScheduling_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)

set(IFXScheduling_SRCS
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/IFXScheduling/IFXSchedulingDllMain.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXClock.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXErrorInfo.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXNotificationInfo.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXNotificationManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXScheduler.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXSchedulerInfo.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXSimulationInfo.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXSimulationManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXSystemManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTaskCallback.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTaskData.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTaskManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTaskManagerNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTaskManagerView.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/CIFXTimeManager.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/IFXScheduling.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Scheduling/IFXSchedulingGuids.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/${U3D_PLATFORM}/Common/IFXOSUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXSubject.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/ModifierChain/CIFXModifier.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/SceneGraph/CIFXMarker.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/WildCards/wcmatch.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
)

add_library(IFXScheduling SHARED 
    ${IFXScheduling_SRCS}
    ${IFXScheduling_HDRS}
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXScheduling/IFXScheduling.rc
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXScheduling/IFXResource.h
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXScheduling/IFXScheduling.def
)
target_link_libraries(IFXScheduling IFXCommon IFXCore)

# IFXRendering (By guess)
GlobSrcRecurse(IFXRendering_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering)
GlobSrcRecurse(IFXRendering_Platform_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering)
GlobSrcRecurse(IFXRendering_Platform_Common_SRCS ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/Common)
GlobHdrRecurse(IFXRendering_Platform_HDRS ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering)
Ubpa_List_Print(TITLE "IFXRendering Sources:" STRS ${IFXRendering_SRCS} PREFIX "  ")
Ubpa_List_Print(TITLE "IFXRendering Platform Common Sources:" STRS ${IFXRendering_Platform_Common_SRCS} PREFIX "  ")
Ubpa_List_Print(TITLE "IFXRendering Platform Sources:" STRS ${IFXRendering_Platform_SRCS} PREFIX "  ")
Ubpa_List_Print(TITLE "IFXRendering Platform Headers:" STRS ${IFXRendering_Platform_HDRS} PREFIX "  ")

set(IFXRendering_HDRS
	${Component_HDRS}
	${Kernel_HDRS}
	${Platform_HDRS}
)

add_library(IFXRendering SHARED
    ${IFXRendering_SRCS}
    ${IFXRendering_Platform_SRCS}
	${IFXRendering_Platform_Common_SRCS}
    ${IFXRendering_HDRS}
    ${IFXRendering_Platform_HDRS}
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Culling/CIFXScenegraphCuller.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Culling/CIFXOctreeCollection.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Culling/CIFXOctreeCuller.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/IFXCorePluginStatic/IFXCorePluginStatic.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/Common/IFXOSRenderWindow.cpp
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering/IFXRendering.rc
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering/IFXResource.h
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering/IFXRendering.def
)

target_include_directories(IFXRendering PUBLIC 
    ${CMAKE_SOURCE_DIR}/Source/RTL/Platform/Win32/IFXRendering/OpenGL
    ${CMAKE_SOURCE_DIR}/Source/RTL/Dependencies/OpenGL/include
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering/OpenGL
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering/DX7
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering/DX8
    ${CMAKE_SOURCE_DIR}/Source/RTL/Component/Rendering/Null
)
target_link_libraries(IFXRendering IFXCore IFXCommon dxsdk ws2_32)

# Sample Player (Win32)
set(SamplePlayer_SRCS
	${IFXRendering_Platform_Common_SRCS}
	${CMAKE_SOURCE_DIR}/Source/Samples/SamplePlayer/SPlayerMain.cpp
	${CMAKE_SOURCE_DIR}/Source/Samples/SampleViewerWin32/U3DViewerWin.cpp
	${CMAKE_SOURCE_DIR}/Source/Samples/SampleViewerWin32/U3DViewer.h
	${CMAKE_SOURCE_DIR}/Source/Samples/SceneHelpers/U3DSceneUtilities.cpp
	${CMAKE_SOURCE_DIR}/Source/Samples/SceneHelpers/U3DSceneUtilities.h
	${CMAKE_SOURCE_DIR}/Source/Samples/SceneHelpers/U3DCameraNavigation.cpp
	${CMAKE_SOURCE_DIR}/Source/Samples/SceneHelpers/U3DCameraNavigation.h

	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreArray.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXCoreList.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXFastAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXListNode.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXMatrix4x4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXQuaternion.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXString.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXUnitAllocator.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector3.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/DataTypes/IFXVector4.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Kernel/Common/IFXDebug.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/CIFXMetaData.cpp
	${CMAKE_SOURCE_DIR}/Source/RTL/Component/Common/IFXDids.cpp
)

add_executable(SampleViewerWin32 WIN32
	${SamplePlayer_SRCS}
	${CMAKE_SOURCE_DIR}/Source/Samples/SampleViewerWin32/SamplePlayer.rc
)

target_include_directories(SampleViewerWin32 PUBLIC ${CMAKE_SOURCE_DIR}/Source/Samples/SceneHelpers)
target_include_directories(SampleViewerWin32 PUBLIC ${CMAKE_SOURCE_DIR}/Source/Samples/SampleViewerWin32)
target_link_libraries(SampleViewerWin32 IFXRendering IFXImporting IFXExporting IFXCore IFXCommon comctl32)