export interface BlogPost {
  slug: string
  title: string
  date: string
}

export interface BlogPostWithContent extends BlogPost {
  content: string
} 