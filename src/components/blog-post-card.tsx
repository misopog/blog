import Link from 'next/link'
import { BlogPost } from '@/types/blog'

interface BlogPostCardProps {
  post: BlogPost
}

export function BlogPostCard({ post }: BlogPostCardProps) {
  return (
    <article className="group relative flex flex-col overflow-hidden rounded-lg border border-zinc-800 bg-zinc-900 transition-all hover:border-zinc-700 hover:bg-zinc-800">
      <div className="flex flex-1 flex-col p-6">
        <div className="flex items-center gap-4 text-sm text-zinc-400">
          <time dateTime={post.date}>
            {new Date(post.date).toLocaleDateString('en-US', {
              year: 'numeric',
              month: 'long',
              day: 'numeric',
            })}
          </time>
        </div>
        <Link href={`/posts/${post.slug}`} className="mt-3">
          <h2 className="text-xl font-semibold text-zinc-100 group-hover:text-white">
            {post.title}
          </h2>
        </Link>
      </div>
    </article>
  )
} 