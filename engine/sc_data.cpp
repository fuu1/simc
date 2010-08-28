#include "simulationcraft.h"

sc_data_t::sc_data_t( sc_data_t* p )
{
  m_spells_index = NULL;
  m_effects_index = NULL;
  m_talents_index = NULL;
  set_parent( p );
}

sc_data_t::sc_data_t( const sc_data_t& copy )
{
  m_spells_index = NULL;
  m_effects_index = NULL;
  m_talents_index = NULL;
  m_parent = copy.m_parent;
  m_copy( copy );
  create_index();
}

void sc_data_t::reset( )
{
  set_parent( (sc_data_t *) m_parent );
}

void sc_data_t::m_copy( const sc_data_t& copy )
{
  m_spells.copy_array( copy.m_spells );
  m_effects.copy_array( copy.m_effects );
  m_talents.copy_array( copy.m_talents );
  m_melee_crit_base.copy_array( copy.m_melee_crit_base );
  m_spell_crit_base.copy_array( copy.m_spell_crit_base );
  m_spell_scaling.copy_array( copy.m_spell_scaling );
  m_melee_crit_scale.copy_array( copy.m_melee_crit_scale );
  m_spell_crit_scale.copy_array( copy.m_spell_crit_scale );
  m_regen_spi.copy_array( copy.m_regen_spi );
  m_octregen.copy_array( copy.m_octregen );
  m_combat_ratings.copy_array( copy.m_combat_ratings );
  m_class_combat_rating_scalar.copy_array( copy.m_class_combat_rating_scalar );
  m_dodge_base.copy_array( copy.m_dodge_base );
  m_dodge_scale.copy_array( copy.m_dodge_scale );
  m_base_mp5.copy_array( copy.m_base_mp5 );
  m_class_stats.copy_array( copy.m_class_stats );
  m_race_stats.copy_array( copy.m_race_stats );
}

void sc_data_t::create_spell_index()
{
  uint32_t max_id = 0;

  if ( m_spells_index )
  {
    delete [] m_spells_index;
    m_spells_index = NULL;
  }

  for ( uint32_t i = 0; i < m_spells.cols; i++ )
  {
    spell_data_t& ref = m_spells.ref( i );
    if ( ref.id > max_id )
    {
      max_id = ref.id;
    }
  }

  if ( max_id >= 1000000 )
  {
    return;
  }

  m_spells_index_size = max_id + 1;

  m_spells_index = new spell_data_t*[ m_spells_index_size ];
  memset( m_spells_index, 0, sizeof( spell_data_t* ) * m_spells_index_size );

  for ( uint32_t i = 0; i < m_spells.cols; i++ )
  {
    spell_data_t* p = m_spells.ptr( i );
    m_spells_index[ p->id ] = p;
  }

  m_spells_index_size = max_id + 1;
}

void sc_data_t::create_effects_index()
{
  uint32_t max_id = 0;

  if ( m_effects_index )
  {
    delete [] m_effects_index;
    m_effects_index = NULL;
  }

  for ( uint32_t i = 0; i < m_effects.cols; i++ )
  {
    spelleffect_data_t& ref = m_effects.ref( i );
    if ( ref.id > max_id )
    {
      max_id = ref.id;
    }
  }

  if ( max_id >= 1000000 )
  {
    return;
  }

  m_effects_index_size = max_id + 1;

  m_effects_index = new spelleffect_data_t*[ m_effects_index_size ];
  memset( m_effects_index, 0, sizeof( spelleffect_data_t* ) * m_effects_index_size );

  for ( uint32_t i = 0; i < m_effects.cols; i++ )
  {
    spelleffect_data_t* p = m_effects.ptr( i );
    m_effects_index[ p->id ] = p;
  }
}

void sc_data_t::create_talents_index()
{
  uint32_t max_id = 0;

  if ( m_talents_index )
  {
    delete [] m_talents_index;
    m_talents_index = NULL;
  }

  for ( uint32_t i = 0; i < m_talents.cols; i++ )
  {
    talent_data_t& ref = m_talents.ref( i );
    if ( ref.id > max_id )
    {
      max_id = ref.id;
    }
  }

  if ( max_id >= 1000000 )
  {
    return;
  }

  m_talents_index_size = max_id + 1;

  m_talents_index = new talent_data_t*[ m_talents_index_size ];
  memset( m_talents_index, 0, sizeof( talent_data_t* ) * m_talents_index_size );

  for ( uint32_t i = 0; i < m_talents.cols; i++ )
  {
    talent_data_t* p = m_talents.ptr( i );
    m_talents_index[ p->id ] = p;
  }

  // Now create the talent trees

  uint32_t max_classes = m_class_stats.rows;
  if ( max_classes == 0 )
    return;

  m_talent_trees.create( MAX_TALENTS, MAX_TALENT_TABS, max_classes );
  m_talent_trees.fill( 0 );

  m_pet_talent_trees.create( MAX_TALENTS, 3 );
  m_pet_talent_trees.fill( 0 );

  uint32_t c, j;


  uint32_t v_size = ( MAX_TALENT_TABS ) * ( max_classes + 3 );
  uint32_t** v1 = new uint32_t*[ v_size ];
  memset( v1, 0, sizeof( uint32_t * ) * v_size );

  uint32_t* n = new uint32_t[ v_size ];
  memset( n, 0, sizeof( uint32_t ) * v_size );


  for ( uint32_t kk = 0; kk < v_size; kk++ )
  {
    v1[ kk ] = new uint32_t[ MAX_TALENTS ];
    memset( v1[ kk ], 0, sizeof( uint32_t ) * MAX_TALENTS );
  }
  
  for ( j = 0; j < max_classes + 3; j++ )
    n[ j ] = 0;

  for ( j = 0; j < m_talents.cols; j++ )
  {
    talent_data_t* p = m_talents.ptr( j );
    if ( !p )
      continue;

    if ( p->id == 11673 )
    {
      int a =1;
      a++;
    }

    if ( p->m_class != 0 )
    {
      for ( c = 1; c < max_classes; c++ )
      {
        if ( ( p->m_class & ( 1 << ( c - 1 ) ) ) == ( uint32_t ) ( 1 << ( c - 1 ) ) )
        {
          if ( p->tab_page < MAX_TALENT_TABS )
          {
            v1[ c * MAX_TALENT_TABS + p->tab_page ][ n[ c * MAX_TALENT_TABS + p->tab_page ] ] = p->id;
            n[ c * MAX_TALENT_TABS + p->tab_page ]++;
          }
        }
      }
    }

    if ( p->m_pet != 0 )
    {
      for ( c = 0; c < 3; c++ )
      {
        if ( ( p->m_pet & ( 1 << ( c - 1 ) ) ) == ( uint32_t ) ( 1 << ( c - 1 ) ) )
        {
          v1[ ( c + max_classes ) * MAX_TALENT_TABS + 0 ][ n[ ( c + max_classes ) * MAX_TALENT_TABS + 0 ] ] = p->id;
          n[ ( c + max_classes ) * MAX_TALENT_TABS + 0 ]++;
        }
      }      
    }
  }

  for ( j = 0; j < max_classes; j++ )
  {
    for ( uint32_t k = 0; k < MAX_TALENT_TABS; k++ )
    {
      sort_talents( v1[ j * MAX_TALENT_TABS + k ], MAX_TALENTS );
//      qsort( v1[ j * MAX_TALENT_TABS + k ], MAX_TALENTS, sizeof( uint32_t ), &sc_data_t::talent_compare );
      for ( uint32_t l = 0; l < n[ j * MAX_TALENT_TABS + k ]; l++ )
      {
        uint32_t* q = m_talent_trees.ptr( l, k, j );

        *q = v1[ j * MAX_TALENT_TABS + k ][ l ];
      }
    }
  }

  for ( j = 0; j < 3; j++ )
  {
    sort_talents( v1[ ( j + max_classes ) * MAX_TALENT_TABS + 0 ], MAX_TALENTS );
    // qsort( v1[ ( j + max_classes ) * MAX_TALENT_TABS + 0 ], MAX_TALENTS, sizeof( uint32_t ), &sc_data_t::talent_compare );
    for ( uint32_t l = 0; l < n[ ( j + max_classes ) * MAX_TALENT_TABS + 0 ]; l++ )
    {
      uint32_t* q = m_pet_talent_trees.ptr( l, j );

      *q = v1[ ( j + max_classes ) * MAX_TALENT_TABS + 0 ][ l ];
    }
  }

  for ( j = 0; j < v_size; j++ )
  {
    delete [] v1[ j ];
  }
  delete [] v1;
  delete [] n;
}

void sc_data_t::create_index()
{
  create_spell_index();
  create_effects_index();
  create_talents_index();  
}

int sc_data_t::talent_compare( const void *vid1, const void *vid2 )
{
  uint32_t id1, id2;

  id1 = *( ( uint32_t* ) vid1 );
  id2 = *( ( uint32_t* ) vid2 );

  assert( m_talents_index && ( id1 < m_talents_index_size ) && ( id2 < m_talents_index_size ) );

  if ( id2 == 0 )
  {
    if ( id1 == 0 )
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
  else if ( id1 == 0 )
  {
    return 1;
  }

  talent_data_t* t1 = m_talents_index[ id1 ];
  talent_data_t* t2 = m_talents_index[ id2 ];

  assert( t1 && t2 );

  if ( t1->row < t2->row )
  {
    return -1;
  }
  else if ( t1->row == t2->row )
  {
    if ( t1->col < t2->col )
    {
      return -1;
    }
    else if ( t1->col == t2->col )
    {
      return 0;
    }
    return 1;
  }
  else
  {
    return 1;
  }
}

void sc_data_t::sort_talents( uint32_t* p, const uint32_t num )
{
  uint32_t i,j;
  uint32_t id1, id2;

  assert( p && num );
  assert( m_talents_index && m_talents_index_size );

  i = 0; j = 0;
  while ( i < ( num - 1 ))
  {
    j = i + 1;
    while ( j < num )
    {
      id1 = p[ i ];
      id2 = p[ j ];
      if ( talent_compare( &id1, &id2 ) > 0 )
      {
        p[ j ] = id1;
        p[ i ] = id2;
      }
      j++;
    }
    i++;
  }
}
