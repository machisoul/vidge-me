---
author: "Vidge Wong"
title: "My tmux config"
date: "2022-09-19"
description: "A comprehensive guide to tmux configuration, focusing on custom key bindings and essential features"
tags: ["shell"]
categories: ["Tools"]
image: img/cover/neofetch.jpg
ShowToc: true
TocOpen: true
---

## What is Tmux?

Tmux (Terminal Multiplexer) is a powerful tool that enables multiple terminal sessions within a single window. It allows you to:
- Split your terminal into multiple panes
- Create multiple windows (like browser tabs)
- Detach and reattach sessions
- Share sessions with other users
- Keep programs running even after disconnecting

## Basic Concepts

1. **Session**: A collection of windows
2. **Window**: Like a tab in your browser, contains one or more panes
3. **Pane**: A split section of a window running a shell or program

## Custom Configuration

Here's a detailed tmux configuration that enhances usability and adds vim-like keybindings:

```bash
# Enable mouse support and focus events
set -g mouse on
set -s focus-events on

# UTF-8 support
set -q -g status-utf8 on
setw -q -g utf8 on

# Key modes
set -g status-keys emacs
set -g mode-keys vi

# Change prefix from 'Ctrl+b' to 'Ctrl+a'
unbind C-b
set -g prefix 'C-a'
```

### Copy Mode Configuration

```bash
# Enter copy mode with Alt+v
bind -n M-v copy-mode
bind p paste-buffer

# Copy to system clipboard (requires xclip)
bind-key -n -T copy-mode-vi Enter send-keys -X copy-pipe 'xclip -i -sel p -f | xclip -i -sel c'
bind-key -n -T copy-mode-vi MouseDragEnd1Pane send-keys -X copy-pipe 'xclip -i -sel p -f | xclip -i -sel c'

# Vim-like copy mode bindings
bind -T copy-mode-vi v send-keys -X begin-selection
bind -T copy-mode-vi C-v send-keys -X rectangle-toggle
bind -T copy-mode-vi K send-keys -N 5 -X cursor-up
bind -T copy-mode-vi J send-keys -N 5 -X cursor-down
bind -T copy-mode-vi H send-keys -X start-of-line
bind -T copy-mode-vi L send-keys -X end-of-line
bind -T copy-mode-vi Y send-keys -X copy-end-of-line
bind -T copy-mode-vi y send-keys -X copy-selection-and-cancel
bind -T copy-mode-vi = send-keys -X search-again
bind -T copy-mode-vi = send-keys -X search-reverse
```

### Navigation Shortcuts

```bash
# Window navigation with Alt+p/n
bind -n M-p previous-window
bind -n M-n next-window

# Pane navigation with Alt+h/j/k/l
bind -n M-h select-pane -L
bind -n M-j select-pane -D
bind -n M-k select-pane -U
bind -n M-l select-pane -R

# Split panes using h/j/k/l
bind k split-window -vb -c "#{pane_current_path}"
bind j split-window -v -c "#{pane_current_path}"
bind h split-window -hb -c "#{pane_current_path}"
bind l split-window -h -c "#{pane_current_path}"
```

## Key Binding Reference

### Basic Operations
- `Ctrl+a`: New prefix key (replaces default `Ctrl+b`)
- `Alt+v`: Enter copy mode
- `p`: Paste buffer

### Window Management
- `Alt+n`: Next window
- `Alt+p`: Previous window

### Pane Navigation
- `Alt+h`: Select left pane
- `Alt+j`: Select down pane
- `Alt+k`: Select up pane
- `Alt+l`: Select right pane

### Pane Splitting
- `Prefix + h`: Split horizontally (left)
- `Prefix + l`: Split horizontally (right)
- `Prefix + j`: Split vertically (down)
- `Prefix + k`: Split vertically (up)

### Copy Mode (Vi-style)
- `v`: Begin selection
- `Ctrl+v`: Rectangle toggle
- `y`: Copy selection
- `