/*
 * \brief   Driver for ARMv7 section tables
 * \author  Martin Stein
 * \date    2012-02-22
 */

/*
 * Copyright (C) 2012 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _INCLUDE__ARM_V7__SECTION_TABLE_H_
#define _INCLUDE__ARM_V7__SECTION_TABLE_H_

/* core includes */
#include <arm/section_table.h>
#include <arm/v7/cpu.h>

namespace Arm_v7
{
	using namespace Genode;

	/**
	 * ARMv7 first level translation table
	 */
	class Section_table : public Arm::Section_table
	{
		public:

			/**
			 * Link to second level translation-table
			 */
			struct Page_table_descriptor : Arm::Section_table::Page_table_descriptor
			{
				struct Ns : Bitfield<3, 1> { }; /* non-secure bit */

				/**
				 * Compose descriptor value
				 */
				static access_t create(Arm::Page_table * const pt,
				                       Section_table * const st)
				{
					return Arm::Section_table::Page_table_descriptor::create(pt) |
					       Ns::bits(!st->secure());
				}
			};

			/**
			 * Section translation descriptor
			 */
			struct Section : Arm::Section_table::Section
			{
				struct Ns : Bitfield<19, 1> { }; /* non-secure bit */

				/**
				 * Compose descriptor value
				 */
				static access_t create(bool const w, bool const x,
				                       bool const k, bool const g,
				                       bool const d, bool const c,
				                       addr_t const pa,
				                       Section_table * const st)
				{
					return Arm::Section_table::Section::create(w, x, k, g, d, c, pa) |
					       Ns::bits(!st->secure());
				}
			};

		protected:

			/* if this table is dedicated to secure mode or to non-secure mode */
			bool _secure;

		public:

			/**
			 * Constructor
			 */
			Section_table() : _secure(Arm_v7::Cpu::secure_mode()) { }

			/**
			 * Insert one atomic translation into this table
			 *
			 * For details see 'Arm::Section_table::insert_translation'
			 */
			unsigned long insert_translation(addr_t const vo, addr_t const pa,
			                                 unsigned long const size_log2,
			                                 bool const w, bool const x,
			                                 bool const k, bool const g,
			                                 bool const d, bool const c,
			                                 void * const extra_space = 0)
			{
				return Arm::Section_table::
				insert_translation<Section_table>(vo, pa, size_log2, w,
				                                  x, k, g, d, c, this,
				                                  extra_space);
			}

			/**
			 * Insert translations for given area, do not permit displacement
			 *
			 * \param vo  virtual offset within this table
			 * \param s   area size
			 * \param d   wether area maps device IO memory
			 * \param c   wether area maps cacheable memory
			 */
			void translate_dpm_off(addr_t vo, size_t s,
			                       bool const d, bool const c)
			{
				Arm::Section_table::
				translate_dpm_off<Section_table>(vo, s, d, c, this);
			}

			/***************
			 ** Accessors **
			 ***************/

			bool secure() { return _secure; }
	};
}


bool Arm::cache_support() { return 1; }


#endif /* _INCLUDE__ARM_V7__SECTION_TABLE_H_ */

