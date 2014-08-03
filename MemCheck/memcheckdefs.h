/**
 * @file
 * @author pavel.iqx
 * @date 2014
 * @copyright GNU General Public License v2
 *
 * Sets some hardcoded constants
 */

#ifndef _MEMCHECKDEFS_H_
#define _MEMCHECKDEFS_H_

#define PLUGIN_PREFIX(...) "[MemCheck] %s", wxString::Format(__VA_ARGS__)
#define BUSY_MESSAGE "Please wait, working..."
#define SUPPRESSION_NAME_PLACEHOLDER "<insert_a_suppression_name_here>"
#define FILTER_NONWORKSPACE_PLACEHOLDER "<nonworkspace_errors>"
#define WAIT_UPDATE_PER_ITEMS 1000
#define ITEMS_FOR_WAIT_DIALOG 5000

#endif
