#pragma once

#include <vector>
#include <wx/string.h>

// A generated file: `name` is a path relative to the skill folder (always '/' separated), `content` is UTF-8 text
struct MetadataFile {
    wxString name;
    wxString content;
};

// Build the files of the wxCrafter AI skill (an agent skill that teaches how to author and edit wxCrafter project
// files, .wxcp):
//
//   SKILL.md                      - the entry point: when to use the skill and the edit -> wxcgen -> verify workflow
//   references/index.md           - how to use the reference + one line per control (m_type ID, class, its file)
//   references/format.md          - the .wxcp JSON layout, the load semantics and a minimal example
//   references/common.md          - what every control has: window styles, sizer flags, properties and events
//   references/<category>.md      - the controls of one category (sizers, containers, ...): for each control its
//                                   styles, sizer flags, properties (label, JSON type, default, options) and events
//
// The reference is not hand-maintained: it is produced by instantiating every control through wxcWidget::Create() and
// reading what the control registered in its constructor, so it always matches the wxCrafter build that produced it.
// It is compact on purpose: it has no descriptions or tooltips.
//
// `version` is the wxCrafter version string, it is written in the header of the index.
std::vector<MetadataFile> BuildMetadataMarkdown(const wxString& version);
