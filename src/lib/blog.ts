import fs from 'fs'
import path from 'path'
import matter from 'gray-matter'
import { BlogPost, BlogPostWithContent } from '@/types/blog'

const postsDirectory = path.join(process.cwd(), 'posts')

export function getAllPosts(): BlogPost[] {
  const fileNames = fs.readdirSync(postsDirectory)
  
  const posts = fileNames.map((fileName) => {
    const fullPath = path.join(postsDirectory, fileName)
    const fileContents = fs.readFileSync(fullPath, 'utf8')
    const { data } = matter(fileContents)
    
    return {
      slug: fileName.replace(/\.md$/, ''),
      title: data.title,
      date: data.date,
    }
  })

  return posts.sort((a, b) => new Date(b.date).getTime() - new Date(a.date).getTime())
}

export function getPostBySlug(slug: string): BlogPostWithContent {
  const fullPath = path.join(postsDirectory, `${slug}.md`)
  const fileContents = fs.readFileSync(fullPath, 'utf8')
  const { data, content } = matter(fileContents)
  
  return {
    slug,
    title: data.title,
    date: data.date,
    content,
  }
}

export function getRelatedPosts(currentSlug: string): BlogPost[] {
  const allPosts = getAllPosts()
  return allPosts
    .filter(post => post.slug !== currentSlug)
    .slice(0, 3)
} 