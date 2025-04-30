#!/bin/bash

# ============================================
# Commit With Custom Author and Committer Info
# Author: Caiden Petty
# ============================================

# Usage:
# ./commit_with_author.sh "Your commit message" "optional-date"
# Example:
# ./commit_with_author.sh "Add new scheduler optimization" "2024-04-01T12:00:00"

# --------------------------------------------
# Configuration (Change if needed)
AUTHOR_NAME="paragonsean"
AUTHOR_EMAIL="cos30degrees@gmail.com"
COMMITTER_NAME="cos30degrees@gmail.com"
COMMITTER_EMAIL="cos30degrees@gmail.com"

# --------------------------------------------
# Arguments
COMMIT_MESSAGE="$1"
COMMIT_DATE="$2"

# --------------------------------------------
# Check for commit message
if [ -z "$COMMIT_MESSAGE" ]; then
  echo "❌ Error: You must provide a commit message."
  echo "Usage: $0 \"Your commit message\" \"optional-date\""
  exit 1
fi

# If no date is given, use current date
if [ -z "$COMMIT_DATE" ]; then
  COMMIT_DATE=$(date -u +"%Y-%m-%dT%H:%M:%SZ")
fi

# --------------------------------------------
# Perform commit
echo "🔵 Committing with:"
echo "Author: $AUTHOR_NAME <$AUTHOR_EMAIL>"
echo "Committer: $COMMITTER_NAME <$COMMITTER_EMAIL>"
echo "Date: $COMMIT_DATE"
echo "Message: \"$COMMIT_MESSAGE\""
echo ""

GIT_AUTHOR_NAME="$AUTHOR_NAME" GIT_AUTHOR_EMAIL="$AUTHOR_EMAIL" \
GIT_COMMITTER_NAME="$COMMITTER_NAME" GIT_COMMITTER_EMAIL="$COMMITTER_EMAIL" \
GIT_AUTHOR_DATE="$COMMIT_DATE" GIT_COMMITTER_DATE="$COMMIT_DATE" \
git commit -m "$COMMIT_MESSAGE"

# Done
echo "✅ Commit complete."

