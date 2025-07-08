---
author: "Vidge Wong"
title: "My vscode config"
date: "2023-09-09"
description: "My vscode config"
categories: ["Tools"]
#aliases: ["migrate-from-jekyl"]
image: img/cover/neofetch.jpg
ShowToc: true
TocOpen: true
---

## Overview

I always wanted to use **vim** in the terminal and didn't want to use vscode. But it was hard to maintain and update the config, so I switched back to **vscode** and now use the vim plugin.

My vscode config is as follows:

```json
{
  "workbench.activityBar.location": "bottom",
  "workbench.colorTheme": "Default Dark+",
  "window.title": "${dirty}${rootName}${separator}${activeEditorMedium}${separator}${appName}",
  "editor.formatOnSave": true,
  "editor.tabSize": 2,
  "editor.cursorSmoothCaretAnimation": "on",
  "editor.minimap.enabled": true,
  "breadcrumbs.enabled": true,
  "editor.detectIndentation": false,

  //vim configuraitons
  "vim.flash.enable": true,
  "vim.useSystemClipboard": true,
  "vim.leader": "<Space>",
  "vim.handleKeys": {
    "<C-d>": true,
    "<C-r>": true,
    "<C-c>": false,
    "<C-x>": false,
    "<C-v>": false,
    "<C-s>": false,
    "<C-z>": false,
    "<C-f>": false,
    "<C-a>": false,
    "<C-p>": false,
    "<C-w>": false,
    "<C-]>": false,
    "<C-[>": false
  },
  // normal mode
  "vim.normalModeKeyBindingsNonRecursive": [
    // NAVIGATION
    // switch b/w buffers
    {
      "before": ["<C-h>"],
      "commands": [":bprevious"]
    },
    {
      "before": ["<C-l>"],
      "commands": [":bnext"]
    },
    // splits
    {
      "before": ["leader", "v"],
      "commands": [":vsplit"]
    },
    {
      "before": ["leader", "s"],
      "commands": [":split"]
    },
    // panes
    {
      "before": ["leader", "h"],
      "commands": ["workbench.action.focusLeftGroup"]
    },
    {
      "before": ["leader", "j"],
      "commands": ["workbench.action.focusBelowGroup"]
    },
    {
      "before": ["leader", "k"],
      "commands": ["workbench.action.focusAboveGroup"]
    },
    {
      "before": ["leader", "l"],
      "commands": ["workbench.action.focusRightGroup"]
    },
    //  vidge wong
    {
      "before": [";"],
      "after": [":"]
    },
    {
      "before": ["K"],
      "after": ["5", "k"]
    },
    {
      "before": ["J"],
      "after": ["5", "j"]
    },
    {
      "before": ["H"],
      "after": ["0", "w"]
    },
    {
      "before": ["L"],
      "after": ["$"]
    }
  ],
  // visual mode
  "vim.visualModeKeyBindingsNonRecursive": [
    {
      "before": [";"],
      "after": [":"]
    },
    {
      "before": ["K"],
      "after": ["5", "k"]
    },
    {
      "before": ["J"],
      "after": ["5", "j"]
    },
    {
      "before": ["H"],
      "after": ["0"]
    },
    {
      "before": ["L"],
      "after": ["$"]
    },
    {
      "before": ["leader"],
      "after": ["<Escape>"]
    }
  ],
  // insert mode
  "vim.insertModeKeyBindingsNonRecursive": [
    {
      "before": ["j", "k"],
      "after": ["<Escape>"]
    }
  ],
  "vim.insertModeKeyBindings": [
    {
      "before": [",", "b"],
      "after": [
        "*",
        "*",
        "*",
        "*",
        " ",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "F",
        "*",
        "h",
        "i"
      ]
    },
    {
      "before": [",", "f"],
      "after": [
        "<Esc>",
        "/",
        "<",
        "+",
        "+",
        ">",
        "<CR>",
        ":",
        "n",
        "o",
        "h",
        "l",
        "s",
        "e",
        "a",
        "r",
        "c",
        "h",
        "<CR>",
        "c",
        "4",
        "l"
      ]
    },
    {
      "before": [",", "s"],
      "after": [
        "~",
        "~",
        "~",
        "~",
        " ",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "F",
        "~",
        "h",
        "i"
      ]
    },
    {
      "before": [",", "i"],
      "after": ["*", "*", " ", "<", "+", "+", ">", "<Esc>", "F", "*", "i"]
    },
    {
      "before": [",", "1"],
      "after": ["#", " ", "<Enter>", "<", "+", "+", ">", "<Esc>", "k", "A"]
    },
    {
      "before": [",", "2"],
      "after": ["#", "#", " ", "<Enter>", "<", "+", "+", ">", "<Esc>", "k", "A"]
    },
    {
      "before": [",", "3"],
      "after": [
        "#",
        "#",
        "#",
        " ",
        "<Enter>",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "k",
        "A"
      ]
    },
    {
      "before": [",", "4"],
      "after": [
        "#",
        "#",
        "#",
        "#",
        " ",
        "<Enter>",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "k",
        "A"
      ]
    },
    {
      "before": [",", "n"],
      "after": ["-", "-", "-", "<Enter>", "<Enter>"]
    },
    {
      "before": [",", "d"],
      "after": ["`", "`", " ", "<", "+", "+", ">", "<Esc>", "F", "`", "i"]
    },
    {
      "before": [",", "c"],
      "after": [
        "`",
        "`",
        "`",
        "<Enter>",
        "<",
        "+",
        "+",
        ">",
        "<Enter>",
        "`",
        "`",
        "`",
        "<Enter>",
        "<Enter>",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "4",
        "k",
        "A"
      ]
    },
    {
      "before": [",", "p"],
      "after": [
        "!",
        "[",
        "]",
        "(",
        "<",
        "+",
        "+",
        ">",
        ")",
        " ",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "F",
        "[",
        "a"
      ]
    },
    {
      "before": [",", "a"],
      "after": [
        "[",
        "]",
        "(",
        "<",
        "+",
        "+",
        ">",
        ")",
        " ",
        "<",
        "+",
        "+",
        ">",
        "<Esc>",
        "F",
        "[",
        "a"
      ]
    },
    {
      "before": [",", "l"],
      "after": ["-", "-", "-", "-", "-", "-", "-", "-", "<Enter>"]
    }
  ],
  "vim.autoSwitchInputMethod.enable": false
}
```
