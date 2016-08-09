
# graph manager and annotation tool

This program is a lightweight `ncurses`-based text-mode tool for navigating and annotating any graph or network. For example, notes for a project, references for an article, or the [Desargues configuration](https://en.wikipedia.org/wiki/Desargues_configuration).

Originally, I started writing this program because I wanted a minimalist personal wiki for organizing notes and references. The other programs I found were too mouse-driven, feature-loaded, or application-specific, and I wanted something simpler than Emacs Org-mode.


(some pictures)


# How it works

The program works best for data with a mutable hierarchy.

The nodes (fields) of your graph belong to disjoint categories. You choose an ordering of the categories and an ordering of the nodes in each category to build a "tree view" into the data.

You can edit fields and category names, add new ones, and link up related nodes.

# Get started

    git clone http://github.com/jimmymathews/gman
    cmake .
    make
    ./gat

# Controls

Since every terminal emulator handles keys differently, a little key-setup program runs the first time `gat` is run. If you make a mistake in the configuration, remove the configuration file `.mk_keys` that it makes in your home directory and it will run again.


- **Ctrl-Up/Down**. Go down to the fields / up to the categories
- **Ctrl-s**. Save to `.graphml` file 
- **Ctrl-o**. Open `.graphml` file (c/o [PugiXML](http://pugixml.org/), in [GraphML](http://graphml.graphdrawing.org/) file format)
- **Esc-Esc**. Exit

Main window

- **Left/Right/Up/Down**. Navigation
- **Shift-Up/Down**. Swap fields in the same category
- **Shift-Left/Right**. Expand or collapse the tree view, or selection if editing field contents
- **Enter**. Start/stop editing a field
- **Delete**. Delete field
- **Home/End**. Navigation if editing field contents
- **Ctrl-c**. Copy selection
- **Ctrl-v**. Paste selection
- **`n`**. New node
- **`l`** then **`s`**.  Link the current node (chosen on **`l`**) to another node (chosen on **`t`**). **Enter** to stop linking.
- **`u`**.  Unlink the current node from its parent

Category bar

- **Left/Right**. Navigation
- **Shift-Left/Right**. Swap categories
- **Enter**. Start/stop editing a category
- **Delete**. Delete category (and all of its field)
- **`n`**. New node belonging to chosen category, linked to chosen node
- **`a`**. Add new category
- **`c`**. Change the color of the fields in the selected category
- **Space**. Hide/unhide the fields of the selected category
