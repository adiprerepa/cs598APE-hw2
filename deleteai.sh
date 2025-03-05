#!/bin/bash
for branch in $(git branch -r | grep -E 'origin/ai[^/]' | sed 's/origin\///'); do
    git push origin --delete "$branch"
done
git branch | grep -E 'ai[^/]' | xargs git branch -D
