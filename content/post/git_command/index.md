---
author: "Vidge Wong"
title: "Common git commands"
date: "2020-03-11"
description: "Some commonly used git commands."
tags: ["git"]
categories: ["Tools"]
#aliases: ["migrate-from-jekyl"]
image: img/cover/neofetch.jpg
ShowToc: true
TocOpen: true
---

## Overview
Git is a widely used version control system for tracking changes in source code during software development. It allows multiple developers to collaborate on a project without interfering with each other's work.

**Key Features:**

1. **Distributed System:** Each developer has a complete copy of the project, enabling work to continue offline and providing backups.
2. **Branching and Merging:** Git makes it easy to create branches for new features or bug fixes, which can be merged back into the main codebase later.
3. **Commit History:** Changes are recorded as "commits," allowing developers to review past changes and revert to previous versions if necessary.
4. **Collaboration:** Developers can push their changes to a shared repository, facilitating teamwork and version control.

Git simplifies the development process, enhancing efficiency and collaboration in coding projects.

## Create repository

```bash
git config --global user.name "<name>"
git config --global user.email "<email>"
git config user.name
git config user.email
ssh-keygen -t rsa -C "<email>"
git clone <url> #clone a remote repository with SSH protocol or HTTPS protocol
git init [folder] #initialize local repository
```

## Modify and submit

```bash
git status #view status
git add .
git rm <file>
git commit -m "<commit message>"
git stash # store work space changes
git stash pop #
```

## View submission history

```bash
git log
git log -p <file> #view the commit log of a given file
```

## Revocation

```bash
git reset --hard HEAD
git reset --hard HEAD^
git reset --hard <commit>
git reset --soft <commit>
git checkout <files>
git restore <files> #revert the files in work space
git restore --staged <files> #revert the files in staged files
```

## Branches and Labels

```bash
git branch
git branch <new branch>
git branch -d <branch>
git branch -m <old branch name> <new branch name>
git checkout <branch>
git checkout -b <new branch> #build and switch to a new branch using the current branch as a prototype
git checkout -b <new branch> origin/<remote branch>
git diff <branch>
git cherry-pick <commit>
```

## Mergers and Derivatives

```bash
git merge <branch>
git merge --strategy-option=theirs <other branch> #Force the merging of other branch into the current branch
git rebase <branch> #equal to <target branch> git merge <current branch>
```

## Remote

```bash
ssh -T <url>
git remote
git remote -v
git remote add <remote> <url>
git pull
git pull <remote> <remote branch>
git checkout -b <local branch> <remote>/<remote branch> #checkout to remote branch
git push
git push <remote> <remote branch>
git push -f <remote> <remote branch> #-f meaning is force
git push <remote> <local branch>:<remote branch>
git push --set-upstream <remote> <remote branch> #Associate the current branch to a remote branch
git push <remote> --delete <remote branch>
git push -f #forced push
git fetch <remote> <remtoe branch>:<local branch>
```

