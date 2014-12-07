//implementation file for INodeEntry

#include "INodeEntry.h"

#ifdef DEBUG
#include <stdio.h>
void printINodeEntry(INodeEntry *entry)
{
  printf("[INodeEntry: id = %d, ref = %d, node = %x, next node = %d]\n",
    entry->_in_id, entry->_in_ref, entry->_in_node, (entry->next == NULL) ? -1 : entry->next->_in_id);
}
#endif