Open from (1) tex file and (2) arxiv link or id (get source).

Configurables
  1. Number of keyword phrases, num_phrases. Configurable at runtime if desired (command line argument?).
  2. Largest word number in keyword phrase, max_words_per_phrase.
  3. Minimum frequency min_freq[n] of phrase of word number n, for n=1,2,..,max_words_per_phrase.
  4. List of weights (summing to 1) for distribution of categories by degree of keyness.
  5. 3 pre-defined configurations; default, thorough, and simple.

Auxiliary functions
  1. Weight function for keyword; degree of keyness. Depends on frequency, number of words, number of times in an emph environment, number of times in a definition/theorem/proposition environment, number of times in a section/subsection/subsubsection. 

Steps
  1. Try to grab title, author, date, proper names, list of environments.
  2. Strip comments and outerier of document environment; gathered from \input commands across several tex files if necessary. Strip enumerate and itemize environment markers, and item markers.
  3. Attempt to render math environments in unicode: subscripts/superscripts and greek letters at least. Elide where not possible. Make list of notations from this, sufficiently short.
  4. Try to strip out all non-math or otherwise handled environment demarcations.
  5. Start priority queue of keyword phrases (configurable size) (by weight function).
  6. Start section, subsection, and subsubsection counters.
  7. Starting with n=max_words_per_phrase, demarcate all n size phrases repeated at least min_freq[n] times and push into queue (evaluate weight function on phrase). For the next (lower) n's, do not look in the previously demarcated areas.
  8. Establish keyword phrase categories, from contents of queue, by distribution list.
  9. Start at beginning of document: One by one unmark the marked phrases, pushing links if the phrase made the cut. When the end (or beginning?) of a subsubsection or subsection or section or environment is reached, push the previous contiguous contents as a text block and link to the parent environment(s?).

Categories
title|person|date|sec|subsec|subsubsec|key1|key2|key3|environment|env num|item num|notation|text
