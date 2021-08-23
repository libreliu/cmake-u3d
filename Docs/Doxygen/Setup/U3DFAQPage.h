//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/**
	@file	U3DFAQPage.h

			This header is used solely to contain the doxygen commands for 
			the Frequently Asked Questions documentation page.
*/


#ifndef U3DFAQPage_H
#define U3DFAQPage_H


//***************************************************************************
//	Doxygen pages
//***************************************************************************


//---------------------------------------------------------------------------
/**
	@page	PageFAQ	Frequently Asked Questions

	These are some Frequently Asked Questions about the Universal 3D Sample 
	Software and the ECMA-363 Universal 3D File Format.

	Special thanks to those that have contributed information to this FAQ.  
	Please continue to submit additions, corrections and suggestions to help 
	improve this FAQ for all users.

	<hr noshade>
	@section SectionContents	Contents

	-	@ref SectionGeneral "General"
		-	@ref Q_General_WhatIsU3DSampleSoftware	"What is the U3D Sample Software?"
		-	@ref Q_General_WhatIsECMA363			"What is ECMA-363?"
		-	@ref Q_General_WhereGetECMA363			"Where can I get ECMA-363?"
		-	@ref Q_General_WhatIsThreadingModel		"What is the U3D Sample Software's threading model?"

	-	@ref SectionConventions	"Conventions"
		-	@ref Q_Conventions_DoConventionsExist	"Are there any coding conventions?"
		-	@ref Q_Conventions_WhatIsNR				"What does @c NR at the end of a function name mean?"
		-	@ref Q_Conventions_WhatIsX				"What does @c X at the end of an interface, component, class or function name mean?"

	-	@ref SectionRendering "Rendering"
		-	@ref Q_Rendering_WhatIsLightingModel	"What is the lighting model?"

	<hr noshade>
	@section SectionGeneral	General

		@subsection Q_General_WhatIsU3DSampleSoftware	What is the U3D Sample Software?
		A set of C++ dynamic link run-time libraries to write, read, extend, 
		render and interact with U3D-formatted data.  It is intended to 
		demonstrate how to perform such activities and to be used as a 
		starting point for tools and applications to support U3D.  Example 
		clients include exporters, converters, importers, authoring packages 
		and interactive viewers.

		@subsection Q_General_WhatIsECMA363	What is ECMA-363?
		It is ECMA International's formal name for the @e Universal @e 3D 
		@e File @e Format specification which describes a 3D file format and 
		the minimal run-time architecture required to support it.  The 
		format is intended for downstream 3D CAD repurposing and 
		visualization.  Salient features include continuous-level-of-detail, 
		compression, support for progressive data streaming and playback, 
		as well as key-frame node and bones-based animation.  2nd edition 
		of the standard provides extensibility of the format and 
		run-time.

		@subsection Q_General_WhereGetECMA363	Where can I get ECMA-363?
		The @e Universal @e 3D @e File @e Format specification is freely 
		available from <a href="http://www.ecma-international.org/default.htm">ECMA 
		International*</a> as standard 
		<a href="http://www.ecma-international.org/publications/standards/Ecma-363.htm">ECMA-363</a>.

		@subsection	Q_General_WhatIsThreadingModel	What is the U3D Sample Software's threading model?
		The U3D Sample Software is currently single-threaded and must be 
		used in a single-threaded manner.

	<hr noshade>
	@section SectionConventions	Conventions

		@subsection Q_Conventions_DoConventionsExist	Are there any coding conventions?
		Conventions for coding the U3D Sample Software do exist.  There 
		are general @ref PageCodingConventions "coding conventions" and also 
		specific ones for designing and using the 
		@ref PageCodingConventionsIFXCOM "IFXCOM component framework".

		@subsection Q_Conventions_WhatIsNR	What does NR at the end of a function name mean?
		Generally, a function name ends with the suffix @c NR if the 
		function breaks the 
		@ref PageCodingConventionsIFXCOM "IFXCOM Coding Conventions" and 
		returns an interface pointer that has @e no @e reference count 
		increase done.  In other words, the interface pointer handed back is 
		a weak reference and it is not necessary to call its 
		@c IFXUnknown::Release function after it is no longer needed.

		@note	Beware, this convention was not always used from the start 
				of the project so it is possible that some functions that 
				hand back weak references to interfaces may not have the 
				@c NR function name suffix.

		@subsection Q_Conventions_WhatIsX	What does X at the end of an interface, component, class or function name mean?
		Generally, an interface, component, class and function name will 
		have an @c X suffix if it throws known exceptions of type 
		@c IFXException.

		@note	Beware, this convention was not always used from the start 
				of the project so it is likely there are some of these 
				without @c X suffixes that do throw @c IFXException 
				exceptions.  The conventions was used most for interfaces 
				and least for functions, components and classes.

		@note	Exception handling was added late in the development of the 
				software code base and thus is not used predominately 
				throughout the software.  It was only fully added for the 
				encoders in the @c IFXExporting and decoders in the 
				@c IFXImporting DLs.

	<hr noshade>
	@section SectionRendering	Rendering

		@subsection Q_Rendering_WhatIsLightingModel	What is the lighting model?
		The lighting model used by renderer is as follows:

		@verbatim
finalColor =   material.emissive
             + material.ambient * global_ambient
             + sum( all light_factors ) @endverbatim

Each @b NON-AMBIENT type light has a @c light_factor:

@verbatim
light_factor[ i ] = 
        attenuation_factor * 
        (   ( material.ambient * light.ambient ) 
          + ( material.diffuse * light.diffuse * ( vertex.normal dot light.direction_from_vertex ) )
          + ( material.specular * light.specular * ( ( vertex.normal dot half_angle ) ^ specular_power ) )
        )

dist = distance from light to vertex

attenuation_factor = 
        1 / ( light.atten[ 0 ] + dist * light.atten[ 1 ] + dist * dist * light.atten[ 2 ] )

half_angle = normalize( light.direction_from_vertex + view.direction_from_vertex )  @endverbatim

        Each light’s color is multiplied by the light intensity before it is 
        submitted to the rendering engine.  So, consider that

        @verbatim
light.ambient   = IFXLight.ambient  * IFXLight.intensity
light.diffuse   = IFXLight.diffuse  * IFXLight.intensity
light.specular  = IFXLight.specular * IFXLight.intensity    @endverbatim

		Lights that have a type of @b AMBIENT at the IFX level have all of 
		their colors summed and set as a global ambient in the rendering 
		engine.

		@verbatim
global_ambient = sum( all IFX lights of type AMBIENT )	@endverbatim

		@note	For @b AMBIENT type lights, the color is not scaled by the 
				intensity factor before being added into the renderer’s 
				global ambient.  So, intensity has no effect on 
				@b AMBIENT lights.

		@note	For @b AMBIENT type lights, the ambient color is actually 
				internally stored in the diffuse variable.
*/


#endif


